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
