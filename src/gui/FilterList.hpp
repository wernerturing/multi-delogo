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
#ifndef MDL_FILTER_LIST_H
#define MDL_FILTER_LIST_H

#include <memory>

#include <gtkmm.h>

#include "FilterListModel.hpp"

#include "filter-generator/FilterList.hpp"


namespace mdl {
  class FilterList : public Gtk::Box
  {
  public:
    FilterList(fg::FilterList& filter_list);

    Glib::RefPtr<FilterListModel> get_model();

    void select(const Gtk::TreeModel::iterator& iter);
    void unselect();

    typedef sigc::signal<void, int> type_signal_selection_changed;
    type_signal_selection_changed signal_selection_changed();

  protected:
    type_signal_selection_changed signal_selection_changed_;

  private:
    Glib::RefPtr<FilterListModel> model_;
    Gtk::TreeView view_;
    Glib::RefPtr<Gtk::TreeView::Selection> selection_;

    void on_selection_changed();
  };
}

#endif // MDL_FILTER_LIST_H
