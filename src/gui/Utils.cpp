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
#include <string>
#include <iomanip>
#include <utility>

#include <gtkmm.h>

#include "Utils.hpp"


namespace mdl {
  static bool confirmation_dialog(Gtk::MessageDialog&& dlg, const Glib::ustring& txt_destructive, const Glib::ustring& txt_safe);
}


bool mdl::file_exists(const std::string& file)
{
  return Glib::file_test(file, Glib::FILE_TEST_EXISTS);
}


bool mdl::confirmation_dialog(const Glib::ustring& msg,
                              const Glib::ustring& txt_destructive,
                              const Glib::ustring& txt_safe)
{
  Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
  return confirmation_dialog(std::move(dlg), txt_destructive, txt_safe);
}


bool mdl::confirmation_dialog(Gtk::Window& parent,
                              const Glib::ustring& msg,
                              const Glib::ustring& txt_destructive,
                              const Glib::ustring& txt_safe)
{
  Gtk::MessageDialog dlg(parent, msg, false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_NONE);
  return confirmation_dialog(std::move(dlg), txt_destructive, txt_safe);
}


bool mdl::confirmation_dialog(Gtk::MessageDialog&& dlg, const Glib::ustring& txt_destructive, const Glib::ustring& txt_safe)
{
  dlg.add_button(txt_destructive, Gtk::RESPONSE_YES);
  dlg.add_button(txt_safe, Gtk::RESPONSE_NO);
  dlg.set_default_response(Gtk::RESPONSE_NO);
  return dlg.run() == Gtk::RESPONSE_YES;
}


std::string mdl::format_time(long ms)
{
  int seconds = ms / 1000;
  ms %= 1000;
  int minutes = seconds / 60;
  seconds %= 60;

  std::ostringstream oss;
  oss << std::setfill('0') << std::setw(2) << minutes
      << ":"
      << std::setfill('0') << std::setw(2) << seconds
      << "."
      << std::setfill('0') << std::setw(3) << ms;

  return oss.str();
}


std::string mdl::format_time_with_hour(long ms)
{
  int seconds = ms / 1000;
  ms %= 1000;
  int minutes = seconds / 60;
  seconds %= 60;
  int hours = minutes / 60;
  minutes %= 60;

  std::ostringstream oss;
  oss << hours
      << ":"
      << std::setfill('0') << std::setw(2) << minutes
      << ":"
      << std::setfill('0') << std::setw(2) << seconds
      << "."
      << std::setfill('0') << std::setw(3) << ms;

  return oss.str();
}


std::string mdl::format_time_based_on_total(long ms, long total_ms)
{
  const long one_hour = 60*60*1000;

  if (total_ms >= one_hour) {
    return format_time_with_hour(ms);
  } else {
    return format_time(ms);
  }
}

