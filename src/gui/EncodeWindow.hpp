/*
 * Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
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

#include "FFmpegExecutor.hpp"


namespace mdl {
  class EncodeWindow : public Gtk::ApplicationWindow
  {
  public:
    EncodeWindow(std::unique_ptr<fg::FilterData> filter_data, int total_frames, double fps);

  private:
    typedef std::shared_ptr<fg::ScriptGenerator> Generator;

    std::unique_ptr<fg::FilterData> filter_data_;
    double fps_;
    FFmpegExecutor::Codec codec_;

    Gtk::Entry txt_file_;
    Gtk::SpinButton txt_quality_;

    Gtk::CheckButton chk_fuzzy_;
    Gtk::SpinButton txt_fuzzyness_;

    Gtk::Box box_progress_;
    Gtk::Label lbl_status_;
    Gtk::ProgressBar progress_bar_;

    std::vector<Gtk::Widget*> widgets_to_disable_;

    FFmpegExecutor ffmpeg_;

    Gtk::Box* create_file_selection();
    Gtk::Box* create_codec();
    Gtk::Box* create_quality();
    Gtk::Box* create_fuzzy();
    Gtk::Box* create_buttons();
    Gtk::Box* create_progress();

    void on_select_file();
    void on_codec(FFmpegExecutor::Codec codec);
    void on_fuzzy_toggled();

    void on_encode();
    void on_generate_script();

    bool check_file(const std::string& file);

    Generator get_generator();

    void on_ffmpeg_progress(const FFmpegExecutor::Progress& p);
    std::string get_progress_str(const FFmpegExecutor::Progress& progress);
    std::string get_time_remaining(int seconds_remaining);

    void on_ffmpeg_finished(bool success, const std::string& error);

    void disable_widgets();
    void enable_widgets();

    bool on_delete_event(GdkEventAny*) override;


    friend class EncodeWindowTestFixture;
  };
}

#endif // MDL_ENCODE_WINDOW_H
