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
#include <boost/algorithm/clamp.hpp>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "common/Exceptions.hpp"
#include "common/FrameProvider.hpp"

#include "FrameNavigator.hpp"
#include "FrameView.hpp"

using namespace mdl;


FrameNavigator::FrameNavigator(Gtk::Window& parent_window,
                               const Glib::RefPtr<FrameProvider>& frame_provider)
  : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
  , parent_window_(parent_window)
  , frame_provider_(frame_provider)
  , number_of_frames_(frame_provider->get_number_of_frames())
  , frame_view_(frame_provider->get_frame_width(), frame_provider->get_frame_height())
  , zoom_(100)
  , lbl_zoom_("100%")
{
  pack_start(frame_view_, true, true);

  Gtk::Box* bottom_box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));
  bottom_box->pack_start(*create_navigation_box(), false, false);
  bottom_box->pack_start(*Gtk::manage(new Gtk::Label()), true, true, 8);
  bottom_box->pack_start(*create_zoom_box(), false, false);

  pack_start(*bottom_box, false, false);
}


Gtk::Box* FrameNavigator::create_navigation_box()
{
  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));

  txt_frame_number_.set_width_chars(6);

  Gtk::Button* btn_prev = Gtk::manage(new Gtk::Button("<"));
  btn_prev->set_tooltip_text(_("Move back one frame (s)"));
  btn_prev->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::single_step_frame),
               -1));

  Gtk::Button* btn_next = Gtk::manage(new Gtk::Button(">"));
  btn_next->set_tooltip_text(_("Move forward one frame (d)"));
  btn_next->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::single_step_frame),
               1));

  Gtk::Button* btn_prev_jump = Gtk::manage(new Gtk::Button("<<"));
  btn_prev_jump->set_tooltip_text(_("Jump back the number of frames specified in \"jump size\" (a)"));
  btn_prev_jump->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::jump_step_frame),
               -1));

  Gtk::Button* btn_next_jump = Gtk::manage(new Gtk::Button(">>"));
  btn_next_jump->set_tooltip_text(_("Jump forward the number of frames specified in \"jump size\" (f)"));
  btn_next_jump->signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::jump_step_frame),
               1));

  txt_frame_number_.set_tooltip_text(_("Current frame number"));
  txt_frame_number_.signal_activate().connect(
    sigc::mem_fun(*this, &FrameNavigator::on_frame_number_activate));
  txt_frame_number_.signal_focus_out_event().connect(
    sigc::mem_fun(*this, &FrameNavigator::on_frame_number_input));

  box->pack_start(*btn_prev_jump, false, false);
  box->pack_start(*btn_prev, false, false);
  box->pack_start(txt_frame_number_, false, false);
  box->pack_start(*Gtk::manage(
    new Gtk::Label(Glib::ustring::compose("/ %1", number_of_frames_))),
    false, false);
  box->pack_start(*btn_next, false, false);
  box->pack_start(*btn_next_jump, false, false);

  Gtk::Label* lbl_jump_size = Gtk::manage(new Gtk::Label(_("_Jump size:"), true));
  lbl_jump_size->set_mnemonic_widget(txt_jump_size_);
  lbl_jump_size->set_margin_start(32);
  box->pack_start(*lbl_jump_size, false, false);

  txt_jump_size_.set_width_chars(6);
  txt_jump_size_.set_tooltip_text(_("Number of frames to jump when using << and >> buttons"));
  box->pack_start(txt_jump_size_, false, false);

  return box;
}


Gtk::Box* FrameNavigator::create_zoom_box()
{
  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));

  btn_zoom_out_.set_image_from_icon_name("zoom-out");
  btn_zoom_out_.set_tooltip_text(_("Make image smaller"));
  box->pack_start(btn_zoom_out_, false, false);

  box->pack_start(lbl_zoom_);

  btn_zoom_in_.set_image_from_icon_name("zoom-in");
  btn_zoom_in_.set_tooltip_text(_("Make image larger"));
  btn_zoom_in_.set_sensitive(false);
  box->pack_start(btn_zoom_in_, false, false);

  btn_zoom_out_.signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::on_zoom),
               -10));
  btn_zoom_in_.signal_clicked().connect(
    sigc::bind(sigc::mem_fun(*this, &FrameNavigator::on_zoom),
               10));

  return box;
}


void FrameNavigator::change_displayed_frame(int new_frame_number)
{
  try {
    new_frame_number = boost::algorithm::clamp(new_frame_number, 1, number_of_frames_);

    auto pixbuf = frame_provider_->get_frame(new_frame_number - 1);
    frame_view_.set_image(pixbuf);

    signal_frame_changed_.emit(new_frame_number);
    frame_number_ = new_frame_number;
    txt_frame_number_.set_value(frame_number_);
  } catch (const FrameNotAvailableException& e) {
    Gtk::MessageDialog dlg(parent_window_,
                           _("Could not get frame"), false,
                           Gtk::MESSAGE_ERROR);
    txt_frame_number_.set_value(frame_number_);
    dlg.run();
  }
}


void FrameNavigator::single_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + direction);
}


void FrameNavigator::jump_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + txt_jump_size_.get_value()*direction);
}


void FrameNavigator::on_frame_number_activate()
{
  change_displayed_frame(txt_frame_number_.get_value());
}


bool FrameNavigator::on_frame_number_input(GdkEventFocus*)
{
  change_displayed_frame(txt_frame_number_.get_value());
  return false;
}


int FrameNavigator::get_jump_size() const
{
  return txt_jump_size_.get_value();
}


void FrameNavigator::set_jump_size(int jump_size)
{
  txt_jump_size_.set_value(jump_size);
}


FrameView& FrameNavigator::get_frame_view()
{
  return frame_view_;
}


FrameNavigator::type_signal_frame_changed FrameNavigator::signal_frame_changed()
{
  return signal_frame_changed_;
}


void FrameNavigator::on_zoom(int increment)
{
  zoom_ += increment;

  btn_zoom_out_.set_sensitive(zoom_ > 10);
  btn_zoom_in_.set_sensitive(zoom_ < 100);

  lbl_zoom_.set_text(Glib::ustring::compose("%1%%", zoom_));

  frame_view_.set_zoom(zoom_);
}
