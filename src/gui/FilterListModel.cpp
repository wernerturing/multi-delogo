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
#include <glibmm/objectbase.h>
#include <glibmm/object.h>
#include <gtkmm/treepath.h>
#include <gtkmm/treemodel.h>

#include "filter-generator/Filters.hpp"
#include "filter-generator/FilterList.hpp"

#include "FilterListModel.hpp"

using namespace mdl;


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


void FilterListModel::get_value_vfunc(const TreeModel::const_iterator& iter, int column, Glib::ValueBase& value) const
{
  throw std::runtime_error("get_value_vfunc not implemented");
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
  throw std::runtime_error("iter_children_vfunc not implemented");
}


bool FilterListModel::iter_has_child_vfunc(const const_iterator& iter) const
{
  throw std::runtime_error("iter_has_child_vfunc not implemented");
}


int FilterListModel::iter_n_children_vfunc(const const_iterator& iter) const
{
  throw std::runtime_error("iter_n_children_vfunc not implemented");
}


int FilterListModel::iter_n_root_children_vfunc() const
{
  return filter_list_.size();
}


bool FilterListModel::iter_nth_child_vfunc(const iterator& parent, int n, iterator& iter) const
{
  throw std::runtime_error("iter_nth_child_vfunc not implemented");
}


bool FilterListModel::iter_nth_root_child_vfunc(int n, iterator& iter) const
{
  throw std::runtime_error("iter_nth_root_child_vfunc not implemented");
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


bool FilterListModel::check_iter_validity(const const_iterator& iter) const
{
  return stamp_ == iter.get_stamp();
}
