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
#ifndef MDL_COORDINATOR_H
#define MDL_COORDINATOR_H

#include <utility>

#include <gtkmm.h>

#include "FilterListModel.hpp"
#include "FilterList.hpp"
#include "FrameNavigator.hpp"
#include "FrameView.hpp"
#include "FilterPanelFactory.hpp"
#include "UndoManager.hpp"


namespace mdl {
  class Coordinator
  {
  public:
    Coordinator(Gtk::Window& parent_window,
                int number_of_frames, int frame_width, int frame_height);

    void set_undo_buttons(Gtk::Widget* btn_undo, Gtk::Widget* btn_redo);
    void set_filter_list(FilterList* filter_list);
    void set_frame_navigator(FrameNavigator* frame_navigator);

    void on_previous_filter();
    void on_next_filter();

    void set_scroll_filter(bool state);

    int get_current_frame();

    void on_undo();
    void on_redo();

  private:
    UndoManager undo_manager_;

    Gtk::Window& parent_window_;

    FilterList* filter_list_;
    Glib::RefPtr<FilterListModel> filter_model_;

    FrameNavigator* frame_navigator_;
    FrameView* frame_view_;
    int current_frame_;
    int number_of_frames_;

    FilterPanelFactory panel_factory_;
    FilterPanel* current_filter_panel_;
    int current_filter_start_frame_;
    fg::filter_ptr current_filter_;
    bool scroll_filter_;


    sigc::connection on_filter_selected_;
    void on_filter_selected(int start_frame);

    void on_frame_changed(int new_frame);

    void select_row(const FilterListModel::iterator& iter);
    void unselect_rows();

    void change_displayed_filter(const FilterListModel::iterator& iter);
    void update_displayed_panel(fg::FilterType type, FilterPanel* panel);
    bool displaying_filter_start_frame();

    sigc::connection on_filter_type_changed_;
    void on_filter_type_changed(fg::FilterType new_type);

    sigc::connection on_frame_rectangle_changed_;
    void on_frame_rectangle_changed(Rectangle rect);
    sigc::connection on_panel_rectangle_changed_;
    void on_panel_rectangle_changed(Rectangle rect);
    void update_filter_for_current_frame();
    void add_new_filter_for_current_frame();
    void update_current_filter();

    sigc::connection on_start_frame_changed_;
    void on_start_frame_changed(int start_frame);
    bool confirm_overwrite_by_start_frame_change(int start_frame);
    void set_start_frame_in_filter_panel(int start_frame);

    void create_new_filter_panel();

    void on_remove_filter();

    void remove_filter(int start_frame);
    void insert_filter(int start_frame, fg::filter_ptr filter);
    void update_filter(int start_frame, fg::filter_ptr filter);
    void change_start_frame(int old_start_frame, int new_start_frame);

    void on_shift();
    std::pair<int, int> shift(int start, int end, int amount);


    friend class AddFilterAction;
    friend class UpdateFilterAction;
    friend class RemoveFilterAction;
    friend class ChangeStartFrameAction;
    friend class ShiftAction;
  };
}

#endif // MDL_COORDINATOR_H
