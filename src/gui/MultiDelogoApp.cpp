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
#include <cerrno>
#include <string>
#include <iostream>
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
#include "Utils.hpp"

using namespace mdl;


const std::string MultiDelogoApp::ACTION_NEW = "app.new";
const std::string MultiDelogoApp::ACTION_OPEN = "app.open";
const std::string MultiDelogoApp::EXTENSION_ = "mdl";


MultiDelogoApp::MultiDelogoApp()
  : Gtk::Application("wt.multi-delogo", Gio::APPLICATION_HANDLES_OPEN)
  , initial_window_(nullptr)
{
  add_action("new", sigc::mem_fun(*this, &MultiDelogoApp::on_new_project));
  add_action("open", sigc::mem_fun(*this, &MultiDelogoApp::on_open_project));

  add_main_option_entry(OPTION_TYPE_BOOL, "version", '\0', _("Outputs application version and exits"));
  add_main_option_entry(OPTION_TYPE_BOOL, "verbose", 'v', _("Outputs debugging information"));
  signal_handle_local_options().connect(sigc::mem_fun(*this, &MultiDelogoApp::handle_options));
}


Glib::RefPtr<MultiDelogoApp> MultiDelogoApp::create()
{
  return Glib::RefPtr<MultiDelogoApp>(new MultiDelogoApp());
}


int MultiDelogoApp::handle_options(const Glib::RefPtr<Glib::VariantDict>& options)
{
  bool version = false;
  options->lookup_value("version", version);
  if (version) {
    std::cout << "multi-delogo version " << PACKAGE_VERSION << std::endl;
    std::cout << "Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>" << std::endl;
    return 0;
  }

  options->lookup_value("verbose", verbose_);

  return -1;
}


void MultiDelogoApp::on_activate()
{
  initial_window_ = InitialWindow::create();
  register_window(initial_window_);
}


void MultiDelogoApp::on_open(const Gio::Application::type_vec_files& files,
                             const Glib::ustring& hint)
{
  for (Glib::RefPtr<Gio::File> file: files) {
    open_file(file);
  }
}


void MultiDelogoApp::open_file(const Glib::RefPtr<Gio::File>& gfile)
{
  open_file(gfile->get_path());
}


void MultiDelogoApp::open_file(const std::string& file)
{
  maybe_Project mpr = open_or_create_project(file);
  if (!mpr) {
    return;
  }

  Glib::RefPtr<FrameProvider> frame_provider = open_movie(*(mpr->filter_data));
  if (!frame_provider) {
    return;
  }

  MovieWindow* window = MovieWindow::create(mpr->file,
                                            std::move(mpr->filter_data),
                                            frame_provider,
                                            *this);
  register_window(window);
}


MultiDelogoApp::maybe_Project MultiDelogoApp::open_or_create_project(const std::string& file)
{
  std::ifstream file_stream(file);
  if (!file_stream.is_open()) {
    auto msg = Glib::ustring::compose(_("Could not open file %1: %2"),
                                      file, Glib::strerror(errno));
    error_dialog(msg);
    return boost::none;
  }

  if (fg::FilterData::is_filter_data(file_stream)) {
    file_stream.seekg(0);
    return open_project(file, file_stream);
  } else {
    return create_project(file);
  }
}


MultiDelogoApp::maybe_Project MultiDelogoApp::open_project(const std::string& project_file, std::istream& project_file_stream)
{
  std::unique_ptr<fg::FilterData> filter_data(new fg::FilterData());
  try {
    filter_data->load(project_file_stream);
  } catch (fg::Exception& e) {
    auto msg = Glib::ustring::compose(_("Invalid data in file %1"), project_file);
    error_dialog(msg);
    return boost::none;
  }

  Project pr{.file = project_file, .filter_data = std::move(filter_data)};
  return pr;
}


MultiDelogoApp::maybe_Project MultiDelogoApp::create_project(const std::string& movie_file)
{
  std::string project_file = movie_file + "." + EXTENSION_;
  if (file_exists(project_file)
      && !confirmation_dialog(Glib::ustring::compose(_("There is already a project corresponding to movie %1. If you start a new project all your previous work will be lost."), movie_file),
                              _("Start a _new project"),
                              _("_Continue existing project"))) {
    open_file(project_file);
    return boost::none;
  }

  std::unique_ptr<fg::FilterData> filter_data(new fg::FilterData());
  filter_data->set_movie_file(movie_file);

  Project pr{.file = project_file,
             .filter_data = std::move(filter_data)};
  return pr;
}


Glib::RefPtr<FrameProvider> MultiDelogoApp::open_movie(fg::FilterData& filter_data)
{
  if (!select_new_movie_file_if_necessary(filter_data)) {
    return Glib::RefPtr<FrameProvider>();
  }

  try {
    return create_frame_provider(filter_data.movie_file());
  } catch (VideoNotOpenedException& e) {
    auto msg = Glib::ustring::compose(_("File %1 not recognized as video or multi-delogo data"), filter_data.movie_file());
    error_dialog(msg);
    return Glib::RefPtr<FrameProvider>();
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


void MultiDelogoApp::on_new_project()
{
  maybe_file file = select_movie_file();
  if (file) {
    open_file(*file);
  }
}


void MultiDelogoApp::on_open_project()
{
  maybe_file file = select_project_file();
  if (file) {
    open_file(*file);
  }
}


maybe_file MultiDelogoApp::select_movie_file()
{
  auto filter_movies = Gtk::FileFilter::create();
  filter_movies->set_name(_("Movies"));
#if !defined(__MINGW32__) && !defined(__APPLE__)
  filter_movies->add_mime_type("video/*");
#else // No wildcard mime support in Windows/Mac
  filter_movies->add_pattern("*.avi");
  filter_movies->add_pattern("*.flv");
  filter_movies->add_pattern("*.mkv");
  filter_movies->add_pattern("*.mp4");
  filter_movies->add_pattern("*.webm");
  filter_movies->add_pattern("*.wmv");
#endif

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


bool MultiDelogoApp::is_verbose() const
{
  return verbose_;
}
