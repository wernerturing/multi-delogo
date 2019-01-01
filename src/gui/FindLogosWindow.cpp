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
#include <memory>
#include <limits>
#include <thread>
#include <mutex>
#include <algorithm>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterData.hpp"

#include "opencv-logo-finder/FilterListAdapter.hpp"

#include "FindLogosWindow.hpp"
#include "ETRProgressBar.hpp"
#include "Utils.hpp"

using namespace mdl;


FindLogosWindow* FindLogosWindow::create(fg::FilterData& filter_data,
                                         int total_frames, int start_frame, int jump_size,
                                         bool verbose)
{
  auto builder = Gtk::Builder::create_from_resource("/wt/multi-delogo/FindLogosWindow.ui");
  FindLogosWindow* window = nullptr;
  builder->get_widget_derived("find_logos_window", window,
                              filter_data, total_frames, start_frame, jump_size,
                              verbose);
  return window;
}


FindLogosWindow::FindLogosWindow(BaseObjectType* cobject,
                                 const Glib::RefPtr<Gtk::Builder>& builder,
                                 fg::FilterData& filter_data,
                                 int total_frames, int start_frame, int jump_size,
                                 bool verbose)
  : MultiDelogoAppWindow(cobject)

  , filter_data_(filter_data)

  , txt_initial_frame_(nullptr)
  , txt_final_frame_(nullptr)
  , txt_min_frame_interval_(nullptr)
  , txt_max_frame_interval_(nullptr)

  , txt_min_logo_width_(nullptr)
  , txt_max_logo_width_(nullptr)
  , txt_min_logo_height_(nullptr)
  , txt_max_logo_height_(nullptr)

  , progress_bar_(nullptr)

  , btn_find_logos_(nullptr)

  , worker_thread_(nullptr)
  , search_in_progress_(false)
  , callback_(finder_progress_dispatcher_)
{
  logo_finder_ = create_logo_finder(filter_data_, callback_, verbose);

  configure_widgets(builder, total_frames, start_frame, jump_size);

  finder_progress_dispatcher_.connect(sigc::mem_fun(*this, &FindLogosWindow::on_progress));
  finder_finished_dispatcher_.connect(sigc::mem_fun(*this, &FindLogosWindow::on_finished));
  callback_.set_finder(logo_finder_.get());
}


void FindLogosWindow::configure_widgets(const Glib::RefPtr<Gtk::Builder>& builder,
                                        int total_frames, int start_frame, int jump_size)
{
  builder->get_widget("txt_initial_frame", txt_initial_frame_);
  configure_spin(*txt_initial_frame_, total_frames - 1);
  txt_initial_frame_->set_value(start_frame);

  builder->get_widget("txt_final_frame", txt_final_frame_);
  configure_spin(*txt_final_frame_, total_frames);
  txt_final_frame_->set_value(std::min(start_frame + 36000, total_frames));

  builder->get_widget("txt_min_frame_interval", txt_min_frame_interval_);
  configure_spin(*txt_min_frame_interval_);
  txt_min_frame_interval_->set_value(jump_size);

  builder->get_widget("txt_max_frame_interval", txt_max_frame_interval_);
  configure_spin(*txt_max_frame_interval_);
  txt_max_frame_interval_->set_value(jump_size);

  builder->get_widget("txt_min_logo_width", txt_min_logo_width_);
  configure_spin(*txt_min_logo_width_);
  txt_min_logo_width_->set_value(logo_finder_->get_min_logo_width());

  builder->get_widget("txt_max_logo_width", txt_max_logo_width_);
  configure_spin(*txt_max_logo_width_);
  txt_max_logo_width_->set_value(logo_finder_->get_max_logo_width());

  builder->get_widget("txt_min_logo_height", txt_min_logo_height_);
  configure_spin(*txt_min_logo_height_);
  txt_min_logo_height_->set_value(logo_finder_->get_min_logo_height());

  builder->get_widget("txt_max_logo_height", txt_max_logo_height_);
  configure_spin(*txt_max_logo_height_);
  txt_max_logo_height_->set_value(logo_finder_->get_max_logo_height());

  builder->get_widget_derived("progress_bar", progress_bar_);

  Gtk::Button* btn_close = nullptr;
  builder->get_widget("btn_close", btn_close);
  btn_close->signal_clicked().connect(sigc::mem_fun(*this, &FindLogosWindow::on_close));

  builder->get_widget("btn_find_logos", btn_find_logos_);
  btn_find_logos_->signal_clicked().connect(sigc::mem_fun(*this, &FindLogosWindow::on_find_logos));
}


void FindLogosWindow::configure_spin(Gtk::SpinButton& spin)
{
  configure_spin(spin, std::numeric_limits<int>::max());
}


void FindLogosWindow::configure_spin(Gtk::SpinButton& spin, int max)
{
  spin.set_range(1, max);
  spin.set_increments(1, 10);
}


void FindLogosWindow::on_find_logos()
{
  int min_frame_interval = txt_min_frame_interval_->get_value_as_int();
  int max_frame_interval = txt_max_frame_interval_->get_value_as_int();
  if (max_frame_interval < min_frame_interval) {
    Gtk::MessageDialog dlg(*this,
                           _("Invalid logo duration: maximum duration must be greater than or than the minimum duration"),
                           false, Gtk::MESSAGE_ERROR);
    dlg.run();
    return;
  }

  if (already_has_filters() && !confirm_search_with_existing_filters()) {
    return;
  }

  int initial_frame = txt_initial_frame_->get_value_as_int() - 1;
  int final_frame = txt_final_frame_->get_value_as_int() - 1;

  logo_finder_->set_start_frame(initial_frame);
  logo_finder_->set_frame_interval_min(min_frame_interval);
  logo_finder_->set_extra_frames(max_frame_interval - min_frame_interval);

  logo_finder_->set_min_logo_width(txt_min_logo_width_->get_value_as_int());
  logo_finder_->set_max_logo_width(txt_max_logo_width_->get_value_as_int());
  logo_finder_->set_min_logo_height(txt_min_logo_height_->get_value_as_int());
  logo_finder_->set_max_logo_height(txt_max_logo_height_->get_value_as_int());

  search_in_progress_ = true;
  callback_.start(initial_frame, final_frame);
  worker_thread_ = new std::thread([this] {
      find_result_ = logo_finder_->find_logos();
      search_in_progress_ = false;
      finder_finished_dispatcher_.emit();
  });
  btn_find_logos_->set_sensitive(false);
}


bool FindLogosWindow::already_has_filters()
{
  int initial_frame = txt_initial_frame_->get_value_as_int();
  int final_frame = txt_final_frame_->get_value_as_int();

  auto filter = filter_data_.filter_list().get_filter_for_frame(final_frame);
  int filter_start_frame = 0;
  if (filter) {
    filter_start_frame = filter->first;
  }

  return filter_start_frame >= initial_frame;
}


bool FindLogosWindow::confirm_search_with_existing_filters()
{
  return confirmation_dialog(*this,
                             _("There are already filters in the range specified for the search. Do you want really want to continue?"),
                             _("_Find logos"), _("_Cancel"));
}


void FindLogosWindow::on_close()
{
  if (confirm_stop()) {
    hide();
  }
}


bool FindLogosWindow::on_delete_event(GdkEventAny*)
{
  bool stop = confirm_stop();
  // Returning false calls the default handler (which closes the window)
  return stop == false;
}


bool FindLogosWindow::confirm_stop()
{
  if (!search_in_progress_) {
    return true;
  }

  bool terminate = confirmation_dialog(*this,
                                       _("Do you really want to stop finding logos?"),
                                       _("_Stop"), _("_Continue"));

  if (terminate) {
    logo_finder_->stop();
  }

  return terminate;
}


void FindLogosWindow::on_progress()
{
  Progress p = callback_.get_progress();
  progress_bar_->set_progress(p);
}


void FindLogosWindow::on_finished()
{
  progress_bar_->set_finished();
  if (!find_result_.first) {
    progress_bar_->set_text(Glib::ustring::compose(_("Process finished unexpectedly: %1"), find_result_.second));
  }
}


FindLogosWindow::~FindLogosWindow()
{
  if (worker_thread_) {
    if (worker_thread_->joinable()) {
      worker_thread_->join();
    }
    delete worker_thread_;
  }
}


FindLogosWindow::ProgressCallback::ProgressCallback(Glib::Dispatcher& dispatcher)
  : dispatcher_(dispatcher)
{
}


void FindLogosWindow::ProgressCallback::success(const mdl::LogoFinderResult& result)
{
  calculate_progress(result.end_frame);
}


void FindLogosWindow::ProgressCallback::failure(int start_frame, int end_frame)
{
  calculate_progress(end_frame);
}


void FindLogosWindow::ProgressCallback::set_finder(LogoFinder* finder)
{
  finder_ = finder;
}


void FindLogosWindow::ProgressCallback::calculate_progress(int end_frame)
{
  if (end_frame > final_frame_) {
    finder_->stop();
    return;
  }

  int total_frames = final_frame_ - initial_frame_;
  int elapsed = end_frame - initial_frame_;

  std::lock_guard<std::mutex> lock(mutex_progress_);

  progress_.percentage = (double) elapsed / total_frames;
  progress_.seconds_elapsed = timer_.elapsed();
  progress_.calculate_time_remaining();

  dispatcher_.emit();
}


void FindLogosWindow::ProgressCallback::start(int initial_frame, int final_frame)
{
  initial_frame_ = initial_frame;
  final_frame_ = final_frame;
  timer_.start();
}


Progress FindLogosWindow::ProgressCallback::get_progress() const
{
  std::lock_guard<std::mutex> lock(mutex_progress_);

  return progress_;
}
