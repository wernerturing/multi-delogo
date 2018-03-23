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
#ifndef MDL_FIND_LOGOS_WINDOW_H
#define MDL_FIND_LOGOS_WINDOW_H

#include <memory>
#include <thread>

#include <gtkmm.h>

#include "filter-generator/FilterData.hpp"

#include "common/LogoFinder.hpp"

#include "MultiDelogoAppWindow.hpp"


namespace mdl {
  class FindLogosWindow : public MultiDelogoAppWindow
  {
  public:
    FindLogosWindow(fg::FilterData& filter_data,
                    int total_frames, int start_frame, int jump_size);
    ~FindLogosWindow();

  private:
    fg::FilterData& filter_data_;
    int total_frames_;
    std::shared_ptr<LogoFinder> logo_finder_;

    Gtk::SpinButton txt_initial_frame_;
    Gtk::SpinButton txt_min_frame_interval_;
    Gtk::SpinButton txt_max_frame_interval_;

    Gtk::SpinButton txt_min_logo_width_;
    Gtk::SpinButton txt_max_logo_width_;
    Gtk::SpinButton txt_min_logo_height_;
    Gtk::SpinButton txt_max_logo_height_;

    Gtk::ProgressBar progress_bar_;

    Gtk::Button btn_find_logos_;

    std::thread* worker_thread_;
    bool search_in_progress_;


    Gtk::Grid* create_parameters();
    Gtk::Grid* create_progress();
    Gtk::Grid* create_buttons();
    void configure_spin(Gtk::SpinButton& spin);

    void on_find_logos();

    void on_close();
    bool on_delete_event(GdkEventAny*) override;
    bool confirm_stop();


    class ProgressCallback : public mdl::LogoFinderCallback
    {
      void success(const mdl::LogoFinderResult& result) override;
      void failure(int start_frame, int end_frame) override;
    };

    ProgressCallback callback_;
  };
}

#endif // MDL_FIND_LOGOS_WINDOW_H
