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
#ifndef MDL_MOVIE_WINDOW_H
#define MDL_MOVIE_WINDOW_H

#include <string>
#include <memory>

#include <gtkmm.h>

#include "common/FrameProvider.hpp"
#include "filter-generator/FilterData.hpp"

#include "MultiDelogoAppWindow.hpp"
#include "FilterList.hpp"
#include "FrameNavigator.hpp"
#include "Coordinator.hpp"


namespace mdl {
  class MovieWindow : public MultiDelogoAppWindow
  {
  public:
    static MovieWindow* create(const std::string& project_file,
                               std::unique_ptr<fg::FilterData> filter_data,
                               const Glib::RefPtr<FrameProvider>& frame_provider,
                               Gtk::Application& app);

    MovieWindow(BaseObjectType* cobject,
                const Glib::RefPtr<Gtk::Builder>& builder,
                const std::string& project_file,
                std::unique_ptr<fg::FilterData> filter_data,
                const Glib::RefPtr<FrameProvider>& frame_provider,
                Gtk::Application& app);

  private:
    Gtk::ToolButton* btn_undo_;
    Gtk::ToolButton* btn_redo_;

    std::string project_file_;
    std::unique_ptr<fg::FilterData> filter_data_;

    FilterList* filter_list_;
    FrameNavigator* frame_navigator_;
    Coordinator coordinator_;

    void configure_toolbar(const Glib::RefPtr<Gtk::Builder>& builder,
                           Gtk::Application& app);

    bool on_key_press(GdkEventKey* key_event);

    void on_save();
    void on_find_logos();
    void on_encode();

    void on_scroll_filter_toggled(Gtk::ToggleToolButton* chk);
    void on_set_prev_frame(Gtk::RadioMenuItem* radio, FrameNavigator::PrevFrame setting);

    void on_hide() override;
  };
}

#endif // MDL_MOVIE_WINDOW_H
