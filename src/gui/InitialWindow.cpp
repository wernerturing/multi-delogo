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
#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "InitialWindow.hpp"
#include "MultiDelogoApp.hpp"

using namespace mdl;


InitialWindow::InitialWindow()
{
  set_title(_("multi-delogo"));
  set_border_width(16);

  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("What would you like to do?")));
  lbl->set_halign(Gtk::ALIGN_START);

  Gtk::Button* btn_new = Gtk::manage(new Gtk::Button(_("Start a _new project"), true));
  btn_new->set_image_from_icon_name("document-new");
  btn_new->set_relief(Gtk::RELIEF_NONE);
  btn_new->set_halign(Gtk::ALIGN_START);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_new->gobj()), MultiDelogoApp::ACTION_NEW.c_str());

  Gtk::Button* btn_open = Gtk::manage(new Gtk::Button(_("_Open an existing project"), true));
  btn_open->set_image_from_icon_name("document-open");
  btn_open->set_relief(Gtk::RELIEF_NONE);
  btn_open->set_halign(Gtk::ALIGN_START);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_open->gobj()), MultiDelogoApp::ACTION_OPEN.c_str());

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 16));
  box->pack_start(*lbl, false, true);
  box->pack_start(*btn_new, true, true);
  box->pack_start(*btn_open, true, true);

  add(*box);
}
