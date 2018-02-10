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

#include "common/Exceptions.hpp"
#include "common/FrameProvider.hpp"

#include "MultiDelogoApp.hpp"
#include "MovieWindow.hpp"

using namespace mdl;


MultiDelogoApp::MultiDelogoApp()
  : Gtk::Application("wt.multi-delogo", Gio::APPLICATION_HANDLES_OPEN) {
}


Glib::RefPtr<MultiDelogoApp> MultiDelogoApp::create() {
  return Glib::RefPtr<MultiDelogoApp>(new MultiDelogoApp());
}


void MultiDelogoApp::on_activate() {
  printf("For now, provide a movie file as argument.\n");
}


void MultiDelogoApp::on_open(const Gio::Application::type_vec_files& files,
                             const Glib::ustring& hint) {
  for (Glib::RefPtr<Gio::File> file: files) {
    create_movie_window(file);
  }
}


void MultiDelogoApp::create_movie_window(const Glib::RefPtr<Gio::File>& file)
{
  try {
    auto frame_provider = create_frame_provider(file->get_path());
    MovieWindow* window = new MovieWindow(frame_provider);
    register_window(window);
  } catch (const VideoNotOpenedException& e) {
    auto msg = Glib::ustring::compose(_("Could not open file %1"), file->get_path());
    Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_ERROR);
    dlg.run();
  }
}


void MultiDelogoApp::register_window(Gtk::ApplicationWindow* window) {
  window->signal_hide().connect(
    sigc::bind<Gtk::ApplicationWindow*>(
      sigc::mem_fun(*this, &MultiDelogoApp::on_hide_window),
      window));

  add_window(*window);
  window->show_all();
  window->present();
}


void MultiDelogoApp::on_hide_window(Gtk::ApplicationWindow* window) {
  delete window;
}
