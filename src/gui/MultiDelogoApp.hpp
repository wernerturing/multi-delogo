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
#ifndef MDL_MULTI_DELOGO_APP_H
#define MDL_MULTI_DELOGO_APP_H

#include <gtkmm.h>

#include <boost/optional.hpp>

#include "filter-generator/FilterData.hpp"

#include "common/FrameProvider.hpp"


namespace mdl {
  typedef boost::optional<Glib::RefPtr<Gio::File>> maybe_file;

  class MultiDelogoApp : public Gtk::Application
  {
  protected:
    MultiDelogoApp();

  public:
    static Glib::RefPtr<MultiDelogoApp> create();

    void save_project(const std::string& project_file,
                      const fg::FilterData& filter_data);

    void register_window(Gtk::ApplicationWindow* window);

    bool is_verbose() const;

    const static std::string ACTION_NEW;
    const static std::string ACTION_OPEN;

  private:
    const static std::string EXTENSION_;

    bool verbose_ = false;

    struct Project
    {
      std::string file;
      std::unique_ptr<fg::FilterData> filter_data;
    };
    typedef boost::optional<Project> maybe_Project;

    Gtk::ApplicationWindow* initial_window_;

    int handle_options(const Glib::RefPtr<Glib::VariantDict>& options);

    void on_activate();

    void on_open(const Gio::Application::type_vec_files& files,
                 const Glib::ustring& hint);
    void open_file(const Glib::RefPtr<Gio::File>& gfile);
    void open_file(const std::string& file);
    maybe_Project open_or_create_project(const std::string& file);
    maybe_Project open_project(const std::string& project_file, std::istream& project_file_stream);
    maybe_Project create_project(const std::string& movie_file);
    Glib::RefPtr<FrameProvider> open_movie(fg::FilterData& filter_data);
    bool select_new_movie_file_if_necessary(fg::FilterData& filter_data);

    void on_new_project();
    void on_open_project();

    maybe_file select_movie_file();
    maybe_file select_project_file();
    maybe_file select_file_for_open(const std::string& title,
                                    const Glib::RefPtr<Gtk::FileFilter>& filter);

    void error_dialog(const Glib::ustring& message, Gtk::MessageType type=Gtk::MESSAGE_ERROR);

    void on_hide_window(Gtk::ApplicationWindow* window);
  };
}

#endif // MDL_MULTI_DELOGO_APP_H
