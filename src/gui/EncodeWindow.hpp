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


namespace mdl {
  class EncodeWindow : public Gtk::ApplicationWindow
  {
  public:
    EncodeWindow(std::unique_ptr<fg::FilterData> filter_data, int total_frames);

  private:
    struct Progress
    {
      double percentage;
      int seconds_elapsed;
    };

    enum class Codec { H264, H265 };
    static const int H264_DEFAULT_CRF_ = 23;
    static const int H265_DEFAULT_CRF_ = 28;

    typedef std::shared_ptr<fg::ScriptGenerator> Generator;

    std::unique_ptr<fg::FilterData> filter_data_;
    int total_frames_;
    Codec codec_;

    Gtk::Entry txt_file_;
    Gtk::SpinButton txt_quality_;

    Gtk::CheckButton chk_fuzzy_;
    Gtk::SpinButton txt_fuzzyness_;

#ifdef __MINGW32__
    Glib::Pid ffmpeg_handle_;
#endif
    std::string tmp_filter_file_;
    Glib::RefPtr<Glib::IOChannel> ffmpeg_out_;
    Glib::Timer ffmpeg_timer_;
    Gtk::Box box_progress_;
    Gtk::Label lbl_status_;
    Gtk::ProgressBar progress_bar_;

    std::vector<Gtk::Widget*> widgets_to_disable_;

    Gtk::Box* create_file_selection();
    Gtk::Box* create_codec();
    Gtk::Box* create_quality();
    Gtk::Box* create_fuzzy();
    Gtk::Box* create_buttons();
    Gtk::Box* create_progress();

    void on_select_file();
    void on_codec(Codec codec);
    void on_fuzzy_toggled();

    void on_encode();
    void on_generate_script();

    bool check_file(const std::string& file);

    Generator get_generator();
    void generate_script(const std::string& file, Generator generator);

    std::vector<std::string> get_ffmpeg_cmd_line(const std::string& filter_file, Generator generator);
    std::vector<std::string> get_audio_opts(Generator generator);
    void start_ffmpeg(const std::vector<std::string>& cmd_line);

    bool on_ffmpeg_output(Glib::IOCondition condition);
    Progress get_progress(const std::string& ffmpeg_stats);
    std::string get_progress_str(const Progress& progress);
    int calculate_seconds_remaining(const Progress& progress);
    std::string get_time_remaining(int seconds_remaining);

    void on_ffmpeg_finished(Glib::Pid pid, int status);

    void disable_widgets();
    void enable_widgets();

    bool on_delete_event(GdkEventAny*) override;


    friend class EncodeWindowTestFixture;
  };
}

#endif // MDL_ENCODE_WINDOW_H
