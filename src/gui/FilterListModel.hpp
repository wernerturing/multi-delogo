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
#ifndef MDL_FILTER_LIST_MODEL_H
#define MDL_FILTER_LIST_MODEL_H

#include <utility>
#include <exception>

#include <glibmm/object.h>
#include <gtkmm/treemodel.h>
#include <gtkmm/treemodelcolumn.h>

#include "filter-generator/Filters.hpp"
#include "filter-generator/FilterList.hpp"


namespace mdl {
  class FilterListColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    Gtk::TreeModelColumn<int> start_frame;
    Gtk::TreeModelColumn<fg::filter_ptr> filter;
    Gtk::TreeModelColumn<Glib::ustring> filter_name;

    FilterListColumns();
  };


  class FilterListModel : public Gtk::TreeModel, public Glib::Object
  {
  protected:
    FilterListModel(fg::FilterList& filter_list);
    virtual ~FilterListModel();

  public:
    static Glib::RefPtr<FilterListModel> create(fg::FilterList& filter_list);

    iterator get_for_frame(int frame);
    iterator get_by_start_frame(int start_frame);

    iterator insert(int start_frame, fg::filter_ptr filter);
    void remove(const iterator& iter);

    std::pair<int, int> shift_frames(int start, int end, int amount);

    static FilterListColumns columns;

  protected:
    Gtk::TreeModelFlags get_flags_vfunc() const override;

    int get_n_columns_vfunc() const override;
    GType get_column_type_vfunc(int index) const override;

    bool iter_next_vfunc(const iterator& iter, iterator& iter_next) const override;
    bool iter_children_vfunc(const iterator& parent, iterator& iter) const override;
    bool iter_has_child_vfunc(const const_iterator& iter) const override;
    int iter_n_children_vfunc(const const_iterator& iter) const override;
    int iter_n_root_children_vfunc() const override;
    bool iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const override;
    bool iter_nth_root_child_vfunc(int n, iterator& iter) const override;
    bool iter_parent_vfunc(const iterator& child, iterator& iter) const override;

    Path get_path_vfunc(const const_iterator& iter) const override;
    bool get_iter_vfunc(const Path& path, iterator& iter) const override;

    void get_value_vfunc(const const_iterator& iter, int column, Glib::ValueBase& value) const override;

    void set_value_impl(const iterator& iter, int column, const Glib::ValueBase& value) override;
    void set_value_start_frame(const iterator& iter, const Glib::ValueBase& value);
    void set_value_filter(const iterator& iter, const Glib::ValueBase& value);


  private:
    fg::FilterList& filter_list_;
    int stamp_;

    int get_position(const iterator& iter) const;
    fg::FilterList::maybe_type get_filter_by_iter(const iterator& iter) const;
    iterator create_iter(int position) const;
    bool check_iter_validity(const const_iterator& iter) const;
  };
}

#endif // MDL_FILTER_LIST_MODEL_H
