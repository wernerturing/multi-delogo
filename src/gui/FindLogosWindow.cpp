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
#include <memory>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterData.hpp"

#include "FindLogosWindow.hpp"

using namespace mdl;


FindLogosWindow::FindLogosWindow(fg::FilterData& filter_data, int total_frames)
  : filter_data_(filter_data)
  , total_frames_(total_frames)
{
  set_title(_("Find logos"));
  set_border_width(8);

  Gtk::Grid* vbox = Gtk::manage(new Gtk::Grid());
  vbox->set_orientation(Gtk::ORIENTATION_VERTICAL);
  vbox->set_row_spacing(8);

  vbox->add(*Gtk::manage(new Gtk::Label("logo finder")));
  vbox->add(*create_buttons());

  add(*vbox);
}


Gtk::Grid* FindLogosWindow::create_buttons()
{
  Gtk::Button* btn_close = Gtk::manage(new Gtk::Button(_("_Close"), true));
  btn_close->signal_clicked().connect(sigc::mem_fun(*this, &Widget::hide));

  Gtk::Grid* box = Gtk::manage(new Gtk::Grid());
  box->set_column_spacing(8);
  box->set_vexpand();
  box->set_valign(Gtk::ALIGN_CENTER);
  box->set_halign(Gtk::ALIGN_END);
  box->add(*btn_close);

  return box;
}
