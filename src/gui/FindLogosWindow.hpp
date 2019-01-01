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
#ifndef MDL_FIND_LOGOS_WINDOW_H
#define MDL_FIND_LOGOS_WINDOW_H

#include <memory>
#include <thread>
#include <mutex>

#include <gtkmm.h>

#include "filter-generator/FilterData.hpp"

#include "common/LogoFinder.hpp"

#include "ETRProgressBar.hpp"
#include "MultiDelogoAppWindow.hpp"


namespace mdl {
  class FindLogosWindow : public MultiDelogoAppWindow
  {
  public:
    static FindLogosWindow* create(fg::FilterData& filter_data,
                                   int total_frames, int start_frame, int jump_size,
                                   bool verbose);

    FindLogosWindow(BaseObjectType* cobject,
                    const Glib::RefPtr<Gtk::Builder>& builder,
                    fg::FilterData& filter_data,
                    int total_frames, int start_frame, int jump_size,
                    bool verbose);
    ~FindLogosWindow();

  private:
    fg::FilterData& filter_data_;
    std::shared_ptr<LogoFinder> logo_finder_;

    Gtk::SpinButton* txt_initial_frame_;
    Gtk::SpinButton* txt_final_frame_;
    Gtk::SpinButton* txt_min_frame_interval_;
    Gtk::SpinButton* txt_max_frame_interval_;

    Gtk::SpinButton* txt_min_logo_width_;
    Gtk::SpinButton* txt_max_logo_width_;
    Gtk::SpinButton* txt_min_logo_height_;
    Gtk::SpinButton* txt_max_logo_height_;

    ETRProgressBar* progress_bar_;

    Gtk::Button* btn_find_logos_;

    std::thread* worker_thread_;
    bool search_in_progress_;
    LogoFinder::find_result find_result_;
    Glib::Dispatcher finder_progress_dispatcher_;
    Glib::Dispatcher finder_finished_dispatcher_;


    void configure_widgets(const Glib::RefPtr<Gtk::Builder>& builder,
                           int total_frames, int start_frame, int jump_size);
    void configure_spin(Gtk::SpinButton& spin);
    void configure_spin(Gtk::SpinButton& spin, int max);

    void on_find_logos();
    bool already_has_filters();
    bool confirm_search_with_existing_filters();

    void on_close();
    bool on_delete_event(GdkEventAny*) override;
    bool confirm_stop();

    void on_progress();

    void on_finished();


    class ProgressCallback : public mdl::LogoFinderCallback
    {
    public:
      ProgressCallback(Glib::Dispatcher& dispatcher);

      void success(const mdl::LogoFinderResult& result) override;
      void failure(int start_frame, int end_frame) override;

      void set_finder(LogoFinder* finder);
      void start(int initial_frame, int final_frame);
      Progress get_progress() const;

    private:
      LogoFinder* finder_;

      Progress progress_;
      mutable std::mutex mutex_progress_;

      int initial_frame_;
      int final_frame_;

      Glib::Timer timer_;

      Glib::Dispatcher& dispatcher_;


      void calculate_progress(int end_frame);
    };

    ProgressCallback callback_;
  };
}

#endif // MDL_FIND_LOGOS_WINDOW_H
