/*
 * Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
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
  auto* d = gtk_editable_get_delegate(GTK_EDITABLE(gobj()));
  delegate_ = dynamic_cast<Gtk::Text*>(Glib::wrap(GTK_WIDGET(d)));
  delegate_->signal_insert_text().connect(
    sigc::mem_fun(*this, &NumericEntry::on_delegate_insert_text), false);
}


void NumericEntry::set_value(int text)
{
  set_text(std::to_string(text));
}


int NumericEntry::get_value() const
{
  return std::stoi(get_text());
}


void NumericEntry::on_delegate_insert_text(const Glib::ustring& text, int*)
{
  if (!contains_only_numbers(text)) {
    g_signal_stop_emission_by_name(delegate_->gobj(), "insert-text");
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
