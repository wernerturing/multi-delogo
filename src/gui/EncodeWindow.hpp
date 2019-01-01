/*
 * Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>
 *
 * This file is part of multi-delogo.
 *
 * multi-delogo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * multi-delogo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MDL_ENCODE_WINDOW_H
#define MDL_ENCODE_WINDOW_H

#include <memory>
#include <string>
#include <vector>

#include <gtkmm.h>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/ScriptGenerator.hpp"

#include "ETRProgressBar.hpp"
#include "MultiDelogoAppWindow.hpp"
#include "FFmpegExecutor.hpp"


namespace mdl {
  class EncodeWindow : public MultiDelogoAppWindow
  {
  public:
    static EncodeWindow* create(std::unique_ptr<fg::FilterData> filter_data,
                                int frame_width, int frame_height,
                                int total_frames, double fps);

    EncodeWindow(BaseObjectType* cobject,
                 const Glib::RefPtr<Gtk::Builder>& builder,
                 std::unique_ptr<fg::FilterData> filter_data,
                 int frame_width, int frame_height,
                 int total_frames, double fps);

  private:
    typedef std::shared_ptr<fg::ScriptGenerator> Generator;

    std::unique_ptr<fg::FilterData> filter_data_;
    int frame_width_;
    int frame_height_;
    double fps_;
    FFmpegExecutor::Codec codec_;

    Gtk::Entry* txt_file_;
    Gtk::SpinButton* txt_quality_;

    Gtk::CheckButton* chk_fuzzy_;
    Gtk::SpinButton* txt_fuzzyness_;

    Gtk::Grid* box_progress_;
    Gtk::Label* lbl_status_;
    ETRProgressBar* progress_bar_;
    Gtk::Button* btn_log_;

    std::vector<Gtk::Widget*> widgets_to_disable_;

    FFmpegExecutor ffmpeg_;

    void configure_widgets(const Glib::RefPtr<Gtk::Builder>& builder);

    void on_select_file();
    void on_codec(FFmpegExecutor::Codec codec);
    void on_fuzzy_toggled();

    void on_encode();
    void on_generate_script();

    bool check_file(const std::string& file);

    Generator get_generator();

    void on_ffmpeg_finished(bool success, const std::string& error);

    void on_view_log();

    void disable_widgets();
    void enable_widgets();

    bool on_delete_event(GdkEventAny*) override;
  };


  class LogWindow : public MultiDelogoAppWindow
  {
  public:
    static LogWindow* create(Gtk::Window& parent, const std::string& log);

    LogWindow(BaseObjectType* cobject,
              const Glib::RefPtr<Gtk::Builder>& builder,
              Gtk::Window& parent, const std::string& log);
  };
}

#endif // MDL_ENCODE_WINDOW_H
