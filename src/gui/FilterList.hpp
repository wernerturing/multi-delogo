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
#ifndef MDL_FILTER_LIST_H
#define MDL_FILTER_LIST_H

#include <memory>

#include <gtkmm.h>

#include "filter-generator/Filters.hpp"
#include "filter-generator/FilterList.hpp"

#include "FilterListModel.hpp"
#include "FilterType.hpp"


namespace mdl {
  class FilterList : public Gtk::Grid
  {
  public:
    FilterList(BaseObjectType* cobject,
               const Glib::RefPtr<Gtk::Builder>& builder,
               fg::FilterList& filter_list);

    Glib::RefPtr<FilterListModel> get_model();

    void select(const Gtk::TreeModel::iterator& iter);
    void unselect();
    Gtk::TreeModel::iterator get_selected() const;
    void scroll_to_row(const Gtk::TreeModel::iterator& iter);

    void set_filter(fg::FilterType filter_type, Gtk::Widget* panel);

    fg::FilterType get_selected_type() const;

    void refresh_list();

    typedef sigc::signal<void, int> type_signal_selection_changed;
    type_signal_selection_changed signal_selection_changed();

    typedef sigc::signal<void> type_signal_button;
    type_signal_button signal_previous_filter();
    type_signal_button signal_next_filter();
    type_signal_button signal_remove_filter();
    type_signal_button signal_shift();

    typedef FilterType::type_signal_type_changed type_signal_type_changed;
    type_signal_type_changed signal_type_changed();

  private:
    Glib::RefPtr<FilterListModel> model_;
    Gtk::TreeView* view_;
    Glib::RefPtr<Gtk::TreeView::Selection> selection_;

    Gtk::Button* btn_remove_filter_;

    FilterType* filter_type_;
    Gtk::Widget* current_panel_;

    type_signal_selection_changed signal_selection_changed_;

    type_signal_button signal_previous_filter_;
    type_signal_button signal_next_filter_;
    type_signal_button signal_remove_filter_;
    type_signal_button signal_shift_;

    type_signal_type_changed signal_type_changed_;


    void configure_buttons(const Glib::RefPtr<Gtk::Builder>& builder);

    void on_selection_changed();
  };
}

#endif // MDL_FILTER_LIST_H
