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
#include <cerrno>
#include <string>
#include <fstream>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/Exceptions.hpp"

#include "common/Exceptions.hpp"
#include "common/FrameProvider.hpp"

#include "MultiDelogoApp.hpp"
#include "InitialWindow.hpp"
#include "MovieWindow.hpp"

using namespace mdl;


const std::string MultiDelogoApp::ACTION_NEW = "app.new";
const std::string MultiDelogoApp::ACTION_OPEN = "app.open";
const std::string MultiDelogoApp::EXTENSION_ = "mdl";


MultiDelogoApp::MultiDelogoApp()
  : Gtk::Application("wt.multi-delogo", Gio::APPLICATION_HANDLES_OPEN)
  , initial_window_(nullptr)
{
  add_action("new", sigc::mem_fun(*this, &MultiDelogoApp::new_project));
  add_action("open", sigc::mem_fun(*this, &MultiDelogoApp::open_project));
}


Glib::RefPtr<MultiDelogoApp> MultiDelogoApp::create()
{
  return Glib::RefPtr<MultiDelogoApp>(new MultiDelogoApp());
}


void MultiDelogoApp::on_activate()
{
  initial_window_ = new InitialWindow();
  register_window(initial_window_);
}


void MultiDelogoApp::on_open(const Gio::Application::type_vec_files& files,
                             const Glib::ustring& hint)
{
  for (Glib::RefPtr<Gio::File> file: files) {
    create_movie_window(file);
  }
}


void MultiDelogoApp::create_movie_window(const Glib::RefPtr<Gio::File>& gfile)
{
  auto file = gfile->get_path();

  std::ifstream file_stream(file);
  if (!file_stream.is_open()) {
    auto msg = Glib::ustring::compose(_("Could not open file %1: %2"),
                                      file, Glib::strerror(errno));
    error_dialog(msg);
    return;
  }

  std::string project_file;
  std::unique_ptr<fg::FilterData> filter_data(new fg::FilterData());
  try {
    filter_data->load(file_stream);
    project_file = file;
  } catch (fg::InvalidFilterDataException& e) {
    file_stream.close();

    filter_data = std::unique_ptr<fg::FilterData>(new fg::FilterData());
    filter_data->set_movie_file(file);

    project_file = file + "." + EXTENSION_;
  } catch (fg::Exception& e) {
    auto msg = Glib::ustring::compose(_("Invalid data in file %1"), file);
    error_dialog(msg);
    return;
  }

  try {
    if (!select_new_movie_file_if_necessary(*filter_data)) {
      return;
    }

    auto frame_provider = create_frame_provider(filter_data->movie_file());
    MovieWindow* window = new MovieWindow(project_file,
                                          std::move(filter_data),
                                          frame_provider);
    register_window(window);
  } catch (const VideoNotOpenedException& e) {
    auto msg = Glib::ustring::compose(_("File %1 not recognized as video or multi-delogo data"), filter_data->movie_file());
    error_dialog(msg);
    return;
  }
}


bool MultiDelogoApp::select_new_movie_file_if_necessary(fg::FilterData& filter_data)
{
  std::ifstream stream(filter_data.movie_file());
  if (stream.is_open()) {
    stream.close();
    return true;
  }

  auto msg = Glib::ustring::compose(_("Movie file %1 in project could not be opened. Please select the movie file."), filter_data.movie_file());
  error_dialog(msg, Gtk::MESSAGE_WARNING);

  maybe_file new_file = select_movie_file();
  if (!new_file) {
    return false;
  }

  filter_data.set_movie_file(new_file.get()->get_path());
  return true;
}


void MultiDelogoApp::register_window(Gtk::ApplicationWindow* window)
{
  if (window != initial_window_ && initial_window_) {
    initial_window_->hide();
    initial_window_ = nullptr; // Will be deleted in on_hide_window
  }

  window->signal_hide().connect(
    sigc::bind(sigc::mem_fun(*this, &MultiDelogoApp::on_hide_window),
               window));

  add_window(*window);
  window->show_all();
  window->present();
}


void MultiDelogoApp::on_hide_window(Gtk::ApplicationWindow* window)
{
  delete window;
}


void MultiDelogoApp::save_project(const std::string& project_file,
                                  const fg::FilterData& filter_data)
{
  std::ofstream file_stream(project_file);
  if (!file_stream.is_open()) {
    auto msg = Glib::ustring::compose(_("Could not open file %1: %2"),
                                      project_file, Glib::strerror(errno));
    error_dialog(msg);
    return;
  }

  filter_data.save(file_stream);
}


void MultiDelogoApp::new_project()
{
  maybe_file file = select_movie_file();
  if (file) {
    create_movie_window(*file);
  }
}


void MultiDelogoApp::open_project()
{
  maybe_file file = select_project_file();
  if (file) {
    create_movie_window(*file);
  }
}


maybe_file MultiDelogoApp::select_movie_file()
{
  auto filter_movies = Gtk::FileFilter::create();
  filter_movies->set_name(_("Movies"));
  filter_movies->add_mime_type("video/*");

  return select_file_for_open(_("Select video file"), filter_movies);
}


maybe_file MultiDelogoApp::select_project_file()
{
  auto filter_mdl = Gtk::FileFilter::create();
  filter_mdl->set_name(_("Project files"));
  filter_mdl->add_pattern(Glib::ustring::compose("*.%1", EXTENSION_));

  return select_file_for_open(_("Open project"), filter_mdl);
}


maybe_file MultiDelogoApp::select_file_for_open(const std::string& title,
                                                const Glib::RefPtr<Gtk::FileFilter>& filter)
{
  Gtk::FileChooserDialog dlg(title);
  if (initial_window_) {
    dlg.set_transient_for(*initial_window_);
  }
  dlg.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
  dlg.add_button(_("_Open"), Gtk::RESPONSE_OK);

  auto filter_all = Gtk::FileFilter::create();
  filter_all->set_name(_("All files"));
  filter_all->add_pattern("*");

  dlg.add_filter(filter);
  dlg.add_filter(filter_all);

  int response = dlg.run();
  dlg.hide();

  if (response == Gtk::RESPONSE_OK) {
    return boost::make_optional(dlg.get_file());
  } else {
    return boost::none;
  }
}


void MultiDelogoApp::error_dialog(const Glib::ustring& message, Gtk::MessageType type)
{
  Gtk::MessageDialog dlg(message, false, type);
  if (initial_window_) {
    dlg.set_transient_for(*initial_window_);
  }
  dlg.run();
}
