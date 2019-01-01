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

#include "NumericEntry.hpp"

using namespace mdl;


NumericEntry::NumericEntry(BaseObjectType* cobject,
                           const Glib::RefPtr<Gtk::Builder>& builder)
  : Gtk::Entry(cobject)
{
}


void NumericEntry::set_value(int text)
{
  set_text(std::to_string(text));
}


int NumericEntry::get_value() const
{
  return std::stoi(get_text());
}


void NumericEntry::on_insert_text(const Glib::ustring& text, int* position)
{
  if (contains_only_numbers(text)) {
    Gtk::Entry::on_insert_text(text, position);
  }
}


bool NumericEntry::contains_only_numbers(const Glib::ustring& text) const
{
  for (auto ch = text.begin(); ch != text.end(); ++ch) {
    if (*ch < '0' || *ch > '9') {
      return false;
    }
  }

  return true;
}
