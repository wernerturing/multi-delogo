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

#include "MovieWindow.hpp"

using namespace mdl;


MovieWindow::MovieWindow(const Glib::RefPtr<FrameProvider>& frame_provider)
  : frame_provider_(frame_provider)
  , number_of_frames_(frame_provider->get_number_of_frames())
{
  set_default_size(600, 600);

  Gtk::Box* vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 8));
  vbox->pack_start(image_, true, true);

  vbox->pack_start(*create_navigation_box(), false, false);

  add(*vbox);

  change_displayed_frame(1000);
  txt_jump_size_.set_value(500);

  signal_key_press_event().connect(sigc::mem_fun(*this, &MovieWindow::on_key_press));
}


Gtk::Box* MovieWindow::create_navigation_box()
{
  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));

  txt_frame_number_.set_width_chars(6);

  Gtk::Button* btn_prev = Gtk::manage(new Gtk::Button("<"));
  btn_prev->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_single_step_frame),
      -1));

  Gtk::Button* btn_next = Gtk::manage(new Gtk::Button(">"));
  btn_next->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_single_step_frame),
      1));

  Gtk::Button* btn_prev_jump = Gtk::manage(new Gtk::Button("<<"));
  btn_prev_jump->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_jump_step_frame),
      -1));

  Gtk::Button* btn_next_jump = Gtk::manage(new Gtk::Button(">>"));
  btn_next_jump->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_jump_step_frame),
      1));

  txt_frame_number_.signal_activate().connect(
    sigc::mem_fun(*this, &MovieWindow::on_frame_number_activate));
  txt_frame_number_.signal_focus_out_event().connect(
    sigc::mem_fun(*this, &MovieWindow::on_frame_number_input));

  box->pack_start(*btn_prev_jump, false, false);
  box->pack_start(*btn_prev, false, false);
  box->pack_start(txt_frame_number_, false, false);
  box->pack_start(*Gtk::manage(
    new Gtk::Label(Glib::ustring::compose("/ %1", number_of_frames_))),
    false, false);
  box->pack_start(*btn_next, false, false);
  box->pack_start(*btn_next_jump, false, false);

  box->pack_start(*Gtk::manage(new Gtk::Label()), false, false, 16);

  Gtk::Label* lbl_jump_size = Gtk::manage(new Gtk::Label(_("_Jump size:"), true));
  lbl_jump_size->set_mnemonic_widget(txt_jump_size_);
  box->pack_start(*lbl_jump_size, false, false);

  txt_jump_size_.set_width_chars(6);
  box->pack_start(txt_jump_size_, false, false);

  return box;
}


void MovieWindow::change_displayed_frame(int new_frame_number)
{
  try {
    new_frame_number = boost::algorithm::clamp(new_frame_number, 1, number_of_frames_);

    auto pixbuf = frame_provider_->get_frame(new_frame_number - 1);
    image_.set(pixbuf);

    frame_number_ = new_frame_number;
    txt_frame_number_.set_value(frame_number_);
  } catch (const FrameNotAvailableException& e) {
    Gtk::MessageDialog dlg(*this,
                           _("Could not get frame"), false,
                           Gtk::MESSAGE_ERROR);
    txt_frame_number_.set_value(frame_number_);
    dlg.run();
  }
}


void MovieWindow::on_single_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + direction);
}


void MovieWindow::on_jump_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + txt_jump_size_.get_value()*direction);
}


void MovieWindow::on_frame_number_activate()
{
  change_displayed_frame(txt_frame_number_.get_value());
}


bool MovieWindow::on_frame_number_input(GdkEventFocus*)
{
  change_displayed_frame(txt_frame_number_.get_value());
  return false;
}


bool MovieWindow::on_key_press(GdkEventKey* key_event)
{
  switch (key_event->keyval) {
  case GDK_KEY_A:
  case GDK_KEY_a:
    on_jump_step_frame(-1);
    return true;

  case GDK_KEY_S:
  case GDK_KEY_s:
    on_single_step_frame(-1);
    return true;

  case GDK_KEY_D:
  case GDK_KEY_d:
    on_single_step_frame(1);
    return true;

  case GDK_KEY_F:
  case GDK_KEY_f:
    on_jump_step_frame(1);
    return true;
  }

  return false;
}
