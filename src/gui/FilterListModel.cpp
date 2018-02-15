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
#include <stdexcept>

#include <glibmm/objectbase.h>
#include <glibmm/object.h>
#include <gtkmm/treepath.h>
#include <gtkmm/treemodel.h>

#include "filter-generator/Filters.hpp"
#include "filter-generator/FilterList.hpp"

#include "FilterListModel.hpp"

using namespace mdl;


FilterListColumns::FilterListColumns()
{
  add(start_frame);
  add(filter);
}


FilterListColumns FilterListModel::columns;


FilterListModel::FilterListModel(fg::FilterList& filter_list)
  : Glib::ObjectBase(typeid(FilterListModel))
  , Glib::Object()
  , filter_list_(filter_list)
  , stamp_(1)
{
}


FilterListModel::~FilterListModel()
{
}


Glib::RefPtr<FilterListModel> FilterListModel::create(fg::FilterList& filter_list)
{
  return Glib::RefPtr<FilterListModel>(new FilterListModel(filter_list));
}


void FilterListModel::insert(int start_frame, fg::Filter* filter)
{
  if (filter_list_.get_by_start_frame(start_frame)) {
    throw DuplicateRowException();
  }

  filter_list_.insert(start_frame, filter);
  ++stamp_;

  int pos = filter_list_.get_position(start_frame);
  Path path;
  path.push_back(pos);
  row_inserted(path, get_iter(path));
}


void FilterListModel::remove(const iterator& iter)
{
  if (!check_iter_validity(iter)) {
    return;
  }

  int pos = GPOINTER_TO_INT(iter.gobj()->user_data);
  fg::FilterList::maybe_type filter = filter_list_.get_by_position(pos);
  if (!pos) {
    return;
  }

  auto path = get_path(iter);
  filter_list_.remove(filter->first);
  ++stamp_;
  row_deleted(path);
}


Gtk::TreeModelFlags FilterListModel::get_flags_vfunc() const
{
  return Gtk::TREE_MODEL_LIST_ONLY;
}


int FilterListModel::get_n_columns_vfunc() const
{
  return 2;
}


GType FilterListModel::get_column_type_vfunc(int index) const
{
  if (index == 0) {
    return Glib::Value<int>::value_type();
  } else if (index == 1) {
    return Glib::Value<fg::Filter*>::value_type();
  } else {
    return 0;
  }
}


bool FilterListModel::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
  iter_next = iterator();

  if (!check_iter_validity(iter)) {
    return false;
  }

  unsigned int next = GPOINTER_TO_INT(iter.gobj()->user_data) + 1;
  if (next > filter_list_.size() - 1) {
    return false;
  }

  iter_next.set_stamp(stamp_);
  iter_next.gobj()->user_data = GINT_TO_POINTER(next);
  return true;
}


bool FilterListModel::iter_children_vfunc(const iterator& parent, iterator& iter) const
{
  return iter_nth_child_vfunc(parent, 0, iter);
}


bool FilterListModel::iter_has_child_vfunc(const const_iterator& iter) const
{
  return false;
}


int FilterListModel::iter_n_children_vfunc(const const_iterator& iter) const
{
  return 0;
}


int FilterListModel::iter_n_root_children_vfunc() const
{
  return filter_list_.size();
}


bool FilterListModel::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
  iter = iterator();
  return false;
}


bool FilterListModel::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
  iter = iterator();

  if ((unsigned) n > filter_list_.size() - 1) {
    return false;
  }

  iter.set_stamp(stamp_);
  iter.gobj()->user_data = GINT_TO_POINTER(n);
  return true;
}


bool FilterListModel::iter_parent_vfunc(const iterator& child, iterator& iter) const
{
  iter = iterator();
  return false;
}

Gtk::TreeModel::Path FilterListModel::get_path_vfunc(const const_iterator& iter) const
{
  Path p;

  if (check_iter_validity(iter)) {
    p.push_back(GPOINTER_TO_INT(iter.gobj()->user_data));
  }

  return p;
}


bool FilterListModel::get_iter_vfunc(const Path& path, iterator& iter) const
{
  iter = iterator();

  auto size = path.size();
  if (size != 1) {
    return false;
  }

  iter.set_stamp(stamp_);
  iter.gobj()->user_data = GINT_TO_POINTER(path[0]);
  return true;
}


void FilterListModel::get_value_vfunc(const const_iterator& iter, int column, Glib::ValueBase& value) const
{
  if (!check_iter_validity(iter)
      || (unsigned) column > columns.size() - 1) {
    return;
  }

  int pos = GPOINTER_TO_INT(iter.gobj()->user_data);
  fg::FilterList::maybe_type filter = filter_list_.get_by_position(pos);
  if (!filter) {
    return;
  }

  if (column == columns.start_frame.index()) {
    g_value_init(value.gobj(), columns.start_frame.type());
    g_value_set_int(value.gobj(), filter->first);
  } else if (column == columns.filter.index()) {
    g_value_init(value.gobj(), columns.filter.type());
    g_value_set_pointer(value.gobj(), filter->second);
  }
}


void FilterListModel::set_value_impl(const iterator& iter, int column, const Glib::ValueBase& value)
{
  if (!check_iter_validity(iter)
      || (unsigned) column > columns.size() - 1
      || !g_value_type_compatible(G_VALUE_TYPE(value.gobj()), get_column_type(column))) {
    return;
  }

  if (column == columns.start_frame.index()) {
    throw std::invalid_argument("Impossible to change start frame");
  } else if (column == columns.filter.index()) {
    Glib::Value<fg::Filter*> filter_value;
    filter_value.init(value.gobj());

    int pos = GPOINTER_TO_INT(iter.gobj()->user_data);
    fg::FilterList::maybe_type filter = filter_list_.get_by_position(pos);
    if (!filter) {
      return;
    }

    filter_list_.insert(filter->first, filter_value.get());

    row_changed(get_path(iter), iter);
  }
}


bool FilterListModel::check_iter_validity(const const_iterator& iter) const
{
  return stamp_ == iter.get_stamp();
}
