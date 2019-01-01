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
#ifndef MDL_NUMERIC_ENTRY_H
#define MDL_NUMERIC_ENTRY_H

#include <gtkmm.h>


namespace mdl {
  class NumericEntry : public Gtk::Entry
  {
  public:
    NumericEntry(BaseObjectType* cobject,
                 const Glib::RefPtr<Gtk::Builder>& builder);

    void set_value(int text);
    int get_value() const;

  protected:
    void on_insert_text(const Glib::ustring& text, int* position) override;

  private:
    bool contains_only_numbers(const Glib::ustring& text) const;
  };
}

#endif // MDL_NUMERIC_ENTRY_H
