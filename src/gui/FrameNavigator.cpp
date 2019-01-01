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
#include <boost/algorithm/clamp.hpp>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "common/Exceptions.hpp"
#include "common/FrameProvider.hpp"

#include "FrameNavigator.hpp"
#include "FrameNavigatorUtil.hpp"
#include "FrameView.hpp"

using namespace mdl;


FrameNavigator::FrameNavigator(BaseObjectType* cobject,
                               const Glib::RefPtr<Gtk::Builder>& builder,
                               Gtk::Window& parent_window,
                               const Glib::RefPtr<FrameProvider>& frame_provider)
  : Gtk::Grid(cobject)
  , parent_window_(parent_window)
  , frame_provider_(frame_provider)
  , number_of_frames_(frame_provider->get_number_of_frames())
  , frame_view_(nullptr)
  , prev_frame_view_(nullptr)
  , lbl_prev_frame_(nullptr)
  , txt_frame_number_(nullptr)
  , txt_jump_size_(nullptr)
  , zoom_(1)
  , lbl_zoom_(nullptr)
  , btn_zoom_out_(nullptr)
  , btn_zoom_in_(nullptr)
  , btn_zoom_100_(nullptr)
{
  builder->get_widget_derived("frame_view", frame_view_,
                              frame_provider_->get_frame_width(), frame_provider_->get_frame_height());
  builder->get_widget_derived("prev_frame_view", prev_frame_view_,
                              frame_provider_->get_frame_width(), frame_provider_->get_frame_height(), false);
  builder->get_widget("lbl_prev_frame", lbl_prev_frame_);

  Glib::signal_idle().connect([&] {
      set_show_prev_frame(PrevFrame::NO);
      return false;
    });

  configure_navigation_bar(builder);
  configure_zoom_bar(builder);

  empty_pixbuf_ = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, false, 8, 1, 1);
}


void FrameNavigator::configure_navigation_bar(const Glib::RefPtr<Gtk::Builder>& builder)
{
  Gtk::Button* btn_prev = nullptr;
  builder->get_widget("btn_prev", btn_prev);
  btn_prev->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::single_step_frame),
               -1));

  Gtk::Button* btn_next = nullptr;
  builder->get_widget("btn_next", btn_next);
  btn_next->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::single_step_frame),
               1));

  Gtk::Button* btn_prev_jump = nullptr;
  builder->get_widget("btn_prev_jump", btn_prev_jump);
  btn_prev_jump->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::jump_step_frame),
               -1));

  Gtk::Button* btn_next_jump = nullptr;
  builder->get_widget("btn_next_jump", btn_next_jump);
  btn_next_jump->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::jump_step_frame),
               1));

  Gtk::Label* lbl_number_of_frames = nullptr;
  builder->get_widget("lbl_number_of_frames", lbl_number_of_frames);
  lbl_number_of_frames->set_text(Glib::ustring::compose("/ %1", number_of_frames_));

  builder->get_widget_derived("txt_frame_number", txt_frame_number_);
  txt_frame_number_->signal_activate().connect(
    sigc::mem_fun(*this, &FrameNavigator::on_frame_number_activate));
  txt_frame_number_->signal_focus_out_event().connect(
    sigc::mem_fun(*this, &FrameNavigator::on_frame_number_input));

  builder->get_widget_derived("txt_jump_size", txt_jump_size_);
}


void FrameNavigator::configure_zoom_bar(const Glib::RefPtr<Gtk::Builder>& builder)
{
  Gtk::Button* btn_zoom_fit = nullptr;
  builder->get_widget("btn_zoom_fit", btn_zoom_fit);
  btn_zoom_fit->signal_clicked().connect(
    sigc::mem_fun(*this, &FrameNavigator::on_zoom_fit));

  builder->get_widget("btn_zoom_out", btn_zoom_out_);
  btn_zoom_out_->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::on_step_zoom),
               -0.1));

  builder->get_widget("btn_zoom_in", btn_zoom_in_);
  btn_zoom_in_->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::on_step_zoom),
               0.1));

  builder->get_widget("btn_zoom_100", btn_zoom_100_);
  btn_zoom_100_->signal_clicked().connect(
    sigc::mem_fun(*this, &FrameNavigator::on_zoom_100));

  builder->get_widget("lbl_zoom", lbl_zoom_);
}


int FrameNavigator::get_number_of_frames() const
{
  return number_of_frames_;
}


int FrameNavigator::get_frame_width() const
{
  return frame_provider_->get_frame_width();
}


int FrameNavigator::get_frame_height() const
{
  return frame_provider_->get_frame_height();
}


double FrameNavigator::get_fps() const
{
  return frame_provider_->get_fps();
}


void FrameNavigator::change_displayed_frame(int new_frame_number)
{
  try {
    new_frame_number = boost::algorithm::clamp(new_frame_number, 1, number_of_frames_);

    if (new_frame_number == frame_number_ + 1) {
      show_next_frame(new_frame_number);
    } else if (new_frame_number == frame_number_ - 1) {
      show_previous_frame(new_frame_number);
    } else if (new_frame_number != frame_number_) {
      show_frame(new_frame_number);
    }

    signal_frame_changed_.emit(new_frame_number);
    frame_number_ = new_frame_number;
    txt_frame_number_->set_value(frame_number_);
  } catch (const FrameNotAvailableException& e) {
    Gtk::MessageDialog dlg(parent_window_,
                           _("Could not get frame"), false,
                           Gtk::MESSAGE_ERROR);
    txt_frame_number_->set_value(frame_number_);
    dlg.run();
  }
}


void FrameNavigator::show_next_frame(int new_frame_number)
{
  prev_frame_pixbuf_ = frame_pixbuf_;
  prev_frame_view_->set_image(frame_pixbuf_);

  fetch_and_show_current_frame(new_frame_number);
}


void FrameNavigator::show_previous_frame(int new_frame_number)
{
  frame_pixbuf_ = prev_frame_pixbuf_;
  frame_view_->set_image(prev_frame_pixbuf_);

  fetch_and_show_prev_frame(new_frame_number);
}


void FrameNavigator::show_frame(int new_frame_number)
{
  fetch_and_show_prev_frame(new_frame_number);
  fetch_and_show_current_frame(new_frame_number);
}


void FrameNavigator::fetch_and_show_current_frame(int new_frame_number)
{
  frame_pixbuf_ = frame_provider_->get_frame(new_frame_number - 1);
  frame_view_->set_image(frame_pixbuf_);
}


void FrameNavigator::fetch_and_show_prev_frame(int new_frame_number)
{
  if (new_frame_number != 1) {
    prev_frame_pixbuf_ = frame_provider_->get_frame(new_frame_number - 2);
    prev_frame_view_->set_image(prev_frame_pixbuf_);
  } else {
    prev_frame_view_->set_image(empty_pixbuf_);
  }
}


void FrameNavigator::single_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + direction);
}


void FrameNavigator::jump_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + txt_jump_size_->get_value()*direction);
}


void FrameNavigator::on_frame_number_activate()
{
  change_displayed_frame(txt_frame_number_->get_value());
}


bool FrameNavigator::on_frame_number_input(GdkEventFocus*)
{
  change_displayed_frame(txt_frame_number_->get_value());
  return false;
}


int FrameNavigator::get_jump_size() const
{
  return txt_jump_size_->get_value();
}


void FrameNavigator::set_jump_size(int jump_size)
{
  txt_jump_size_->set_value(jump_size);
}


void FrameNavigator::set_show_prev_frame(PrevFrame setting)
{
  prev_frame_view_on_size_allocate_.disconnect();

  switch (setting) {
  case PrevFrame::NO:
    break;

  case PrevFrame::FIT:
    prev_frame_view_->set_vadjustment(Gtk::Adjustment::create(0, 1, 1));
    prev_frame_view_->set_hadjustment(Gtk::Adjustment::create(0, 1, 1));
    prev_frame_view_on_size_allocate_ = prev_frame_view_->signal_size_allocate().connect(sigc::mem_fun(*this, &FrameNavigator::set_prev_frame_zoom));
    break;

  case PrevFrame::SAME:
    prev_frame_view_->set_vadjustment(frame_view_->get_vadjustment());
    prev_frame_view_->set_hadjustment(frame_view_->get_hadjustment());
    prev_frame_view_->set_zoom(zoom_);
    break;
  }

  lbl_prev_frame_->set_visible(setting != PrevFrame::NO);
  prev_frame_view_->set_visible(setting != PrevFrame::NO);

  prev_frame_setting_ = setting;
}


FrameView* FrameNavigator::get_frame_view()
{
  return frame_view_;
}


FrameNavigator::type_signal_frame_changed FrameNavigator::signal_frame_changed()
{
  return signal_frame_changed_;
}


void FrameNavigator::on_step_zoom(gdouble increment)
{
  set_zoom(boost::algorithm::clamp(zoom_ + increment, 0.1, 1.0));
}


void FrameNavigator::on_zoom_100()
{
  set_zoom(1);
}


void FrameNavigator::on_zoom_fit()
{
  Gtk::Allocation size = frame_view_->get_allocation();
  set_zoom(get_zoom_to_fit_ratio(frame_provider_->get_frame_width(), frame_provider_->get_frame_height(),
                                  size.get_width(), size.get_height()));
}


void FrameNavigator::set_zoom(gdouble zoom)
{
  zoom_ = zoom;

  btn_zoom_out_->set_sensitive(zoom_ > 0.1);
  btn_zoom_in_->set_sensitive(zoom_ < 1.0);
  btn_zoom_100_->set_sensitive(zoom_ != 1.0);

  lbl_zoom_->set_text(Glib::ustring::compose("%1%%", (int) (zoom_ * 100)));

  frame_view_->set_zoom(zoom_);
  if (prev_frame_setting_ == PrevFrame::SAME) {
    prev_frame_view_->set_zoom(zoom_);
  }
}


void FrameNavigator::set_prev_frame_zoom(Gtk::Allocation size)
{
  gdouble ratio = get_zoom_to_fit_ratio(frame_provider_->get_frame_width(), frame_provider_->get_frame_height(),
                                       size.get_width(), size.get_height());
  prev_frame_view_->set_zoom(ratio);
}
