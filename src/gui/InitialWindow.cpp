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
#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "InitialWindow.hpp"
#include "MultiDelogoApp.hpp"

using namespace mdl;


InitialWindow* InitialWindow::create()
{
  auto builder = Gtk::Builder::create_from_resource("/wt/multi-delogo/InitialWindow.ui");
  InitialWindow* window = nullptr;
  builder->get_widget_derived("initial_window", window);
  return window;
}


InitialWindow::InitialWindow(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
  : MultiDelogoAppWindow(cobject)
{
  Gtk::Button* btn_new = nullptr;
  builder->get_widget("btn_new", btn_new);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_new->gobj()), MultiDelogoApp::ACTION_NEW.c_str());

  Gtk::Button* btn_open = nullptr;
  builder->get_widget("btn_open", btn_open);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_open->gobj()), MultiDelogoApp::ACTION_OPEN.c_str());
}
