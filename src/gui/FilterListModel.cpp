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
#include <utility>
#include <stdexcept>

#include <glibmm/objectbase.h>
#include <glibmm/object.h>
#include <gtkmm/treepath.h>
#include <gtkmm/treemodel.h>

#include "filter-generator/Filters.hpp"
#include "filter-generator/FilterList.hpp"

#include "common/Exceptions.hpp"

#include "FilterListModel.hpp"

using namespace mdl;


FilterListColumns::FilterListColumns()
{
  add(start_frame);
  add(filter);
  add(filter_name);
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


FilterListModel::iterator FilterListModel::get_for_frame(int frame)
{
  auto my_children = children();
  fg::FilterList::maybe_type filter = filter_list_.get_filter_for_frame(frame);
  if (!filter) {
    return my_children.end();
  }

  int pos = filter_list_.get_position(filter->first);
  return my_children[pos];
}


FilterListModel::iterator FilterListModel::get_by_start_frame(int start_frame)
{
  auto children_col = children();
  int pos = filter_list_.get_position(start_frame);
  if (pos == -1) {
    return children_col.end();
  }

  return children_col[pos];
}


FilterListModel::iterator FilterListModel::insert(int start_frame, fg::filter_ptr filter)
{
  if (filter_list_.get_by_start_frame(start_frame)) {
    throw DuplicateRowException();
  }

  filter_list_.insert(start_frame, filter);
  ++stamp_;

  int pos = filter_list_.get_position(start_frame);
  Path path;
  path.push_back(pos);
  auto iter = get_iter(path);
  row_inserted(path, iter);

  return iter;
}


void FilterListModel::remove(const iterator& iter)
{
  if (!check_iter_validity(iter)) {
    g_warning("FilterListModel::remove with invalid iter");
    return;
  }

  fg::FilterList::maybe_type filter = get_filter_by_iter(iter);
  if (!filter) {
    g_warning("FilterListModel::remove: no filter found");
    return;
  }

  auto path = get_path(iter);
  filter_list_.remove(filter->first);
  ++stamp_;
  row_deleted(path);
}


std::pair<int, int> FilterListModel::shift_frames(int start, int end, int amount)
{
  iterator iter;
  if (amount > 0) {
    iter = get_for_frame(end);
  } else {
    iter = get_for_frame(start);
    if (!iter) {
      iter = children()[0];
    }

    if (iter && (*iter)[columns.start_frame] < start) {
      ++iter;
    }
  }

  if (!iter) {
    return std::make_pair(0, 0);
  }

  auto path = get_path(iter);
  int first_frame = (*iter)[columns.start_frame];
  int last_frame;

  int iter_start_frame = first_frame;
  while (true) {
    last_frame = iter_start_frame;
    (*iter)[columns.start_frame] = iter_start_frame + amount;

    if (amount > 0) {
      if (!path.prev()) {
        break;
      }
    } else {
      path.next();
    }
    iter = get_iter(path);
    if (!iter) {
      break;
    }

    iter_start_frame = (*iter)[columns.start_frame];
    if (iter_start_frame < start || iter_start_frame > end) {
      break;
    }
  }

  if (amount > 0) {
    return std::make_pair(last_frame, first_frame);
  } else {
    return std::make_pair(first_frame, last_frame);
  }
}


Gtk::TreeModelFlags FilterListModel::get_flags_vfunc() const
{
  return Gtk::TREE_MODEL_LIST_ONLY;
}


int FilterListModel::get_n_columns_vfunc() const
{
  return columns.size();
}


GType FilterListModel::get_column_type_vfunc(int index) const
{
  if ((unsigned) index + 1 > columns.size()) {
    g_warning("Invalid column %d for FilterListModel::get_column_type_vfunc", index);
    return 0;
  }

  return columns.types()[index];
}


bool FilterListModel::iter_next_vfunc(const iterator& iter, iterator& iter_next) const
{
  iter_next = iterator();

  if (!check_iter_validity(iter)) {
    return false;
  }

  unsigned int next = get_position(iter) + 1;
  if (next + 1 > filter_list_.size()) {
    return false;
  }

  iter_next = create_iter(next);
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

  if ((unsigned) n + 1 > filter_list_.size()) {
    return false;
  }

  iter = create_iter(n);
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
    p.push_back(get_position(iter));
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

  unsigned int pos = path[0];
  if (pos + 1 > filter_list_.size()) {
    return false;
  }

  iter = create_iter(pos);
  return true;
}


void FilterListModel::get_value_vfunc(const const_iterator& iter, int column, Glib::ValueBase& value) const
{
  if (!check_iter_validity(iter)
      || (unsigned) column > columns.size() - 1) {
    g_warning("FilterListModel::get_value_vfunc: invalid iter or column");
    return;
  }

  fg::FilterList::maybe_type filter = get_filter_by_iter(iter);
  if (!filter) {
    g_warning("FilterListModel::get_value_vfunc: no filter found");
    return;
  }

  g_value_init(value.gobj(), get_column_type(column));
  if (column == columns.start_frame.index()) {
    g_value_set_int(value.gobj(), filter->first);
  } else if (column == columns.filter.index()) {
    Glib::Value<fg::filter_ptr> value_specific;
    value_specific.init(Glib::Value<fg::filter_ptr>::value_type());
    value_specific.set(filter->second);
    value = value_specific;
  } else if (column == columns.filter_name.index()) {
    g_value_set_string(value.gobj(), filter->second->name().c_str());
  }
}


void FilterListModel::set_value_impl(const iterator& iter, int column, const Glib::ValueBase& value)
{
  if (!check_iter_validity(iter)
      || (unsigned) column > columns.size() - 1
      || !g_value_type_compatible(G_VALUE_TYPE(value.gobj()), get_column_type(column))) {
    g_warning("FilterListModel::set_value_impl: invalid iter or column; or incompatible g_value");
    return;
  }

  if (column == columns.start_frame.index()) {
    set_value_start_frame(iter, value);
  } else if (column == columns.filter.index()) {
    set_value_filter(iter, value);
  } else if (column == columns.filter_name.index()) {
    throw std::invalid_argument("Impossible to change filter name");
  }
}


void FilterListModel::set_value_start_frame(const iterator& iter, const Glib::ValueBase& value)
{
  Glib::Value<int> start_frame_value;
  start_frame_value.init(value.gobj());
  int new_start_frame = start_frame_value.get();
  int current_start_frame = (*iter)[columns.start_frame];

  auto existing = get_by_start_frame(new_start_frame);
  if (existing) {
    remove(existing);
  }

  auto removed_path = get_path(get_by_start_frame(current_start_frame));

  filter_list_.change_start_frame(current_start_frame, new_start_frame);
  ++stamp_;

  row_deleted(removed_path);

  auto new_iter = get_by_start_frame(new_start_frame);
  row_inserted(get_path(new_iter), new_iter);
}


void FilterListModel::set_value_filter(const iterator& iter, const Glib::ValueBase& value)
{
  Glib::Value<fg::filter_ptr> filter_value;
  filter_value.init(value.gobj());

  fg::FilterList::maybe_type filter = get_filter_by_iter(iter);
  if (!filter) {
    g_warning("FilterListModel::set_value_impl: filter_not_found");
    return;
  }

  filter_list_.insert(filter->first, filter_value.get());

  row_changed(get_path(iter), iter);
}


int FilterListModel::get_position(const iterator& iter) const
{
  return GPOINTER_TO_INT(iter.gobj()->user_data);
}


fg::FilterList::maybe_type FilterListModel::get_filter_by_iter(const iterator& iter) const
{
  int pos = get_position(iter);
  return filter_list_.get_by_position(pos);
}


FilterListModel::iterator FilterListModel::create_iter(int position) const
{
  iterator iter;
  iter.set_stamp(stamp_);
  iter.gobj()->user_data = GINT_TO_POINTER(position);
  return iter;
}


bool FilterListModel::check_iter_validity(const const_iterator& iter) const
{
  return stamp_ == iter.get_stamp();
}
