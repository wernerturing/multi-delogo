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
#ifndef MDL_COORDINATOR_H
#define MDL_COORDINATOR_H

#include <gtkmm.h>

#include "FilterListModel.hpp"
#include "FilterList.hpp"
#include "FrameNavigator.hpp"
#include "FrameView.hpp"
#include "FilterPanelFactory.hpp"


namespace mdl {
  class Coordinator
  {
  public:
    Coordinator(FilterList& filter_list,
                FrameNavigator& frame_navigator,
                int frame_width, int frame_height);

    void update_current_filter_if_necessary();

  private:
    FilterList& filter_list_;
    Glib::RefPtr<FilterListModel> filter_model_;

    FrameNavigator& frame_navigator_;
    FrameView& frame_view_;
    int current_frame_;

    FilterPanelFactory panel_factory_;
    FilterPanel* current_filter_panel_;
    fg::Filter* current_filter_;

    sigc::connection on_filter_selected_;
    void on_filter_selected(int start_frame);

    void on_frame_changed(int new_frame);

    void select_row(const FilterListModel::iterator& iter);
    void unselect_rows();

    void change_displayed_filter(const FilterListModel::iterator& iter);
    void update_displayed_panel(fg::FilterType type, FilterPanel* panel);

    sigc::connection on_frame_rectangle_changed_;
    void on_frame_rectangle_changed(Rectangle rect);
    sigc::connection on_panel_rectangle_changed_;
    void on_panel_rectangle_changed(Rectangle rect);

    void create_new_filter_panel();
    void add_new_filter_if_not_on_filter_starting_frame();

    void on_remove_filter();
  };
}

#endif // MDL_COORDINATOR_H
