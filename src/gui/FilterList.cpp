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
#include <memory>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "FilterList.hpp"
#include "FilterListModel.hpp"

#include "filter-generator/FilterList.hpp"

using namespace mdl;


FilterList::FilterList(fg::FilterList& filter_list)
  : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
  , model_(FilterListModel::create(filter_list))
  , view_(model_)
  , current_panel_(nullptr)
{
  view_.append_column(_("Start frame"), model_->columns.start_frame);
  view_.append_column(_("Filter"), model_->columns.filter_name);

  Gtk::ScrolledWindow* scroll = Gtk::manage(new Gtk::ScrolledWindow());
  scroll->add(view_);

  pack_start(*scroll, false, false);

  selection_ = view_.get_selection();
  selection_->signal_changed().connect(sigc::mem_fun(*this, &FilterList::on_selection_changed));
}


Glib::RefPtr<FilterListModel> FilterList::get_model()
{
  return model_;
}


void FilterList::select(const Gtk::TreeModel::iterator& iter)
{
  selection_->select(iter);
}


void FilterList::unselect()
{
  selection_->unselect_all();
}


void FilterList::set_filter_panel(Gtk::Widget* panel)
{
  if (current_panel_) {
    remove(*current_panel_);
  }

  pack_start(*panel, true, true);
  show_all();
  current_panel_ = panel;
}


FilterList::type_signal_selection_changed FilterList::signal_selection_changed()
{
  return signal_selection_changed_;
}


void FilterList::on_selection_changed()
{
  auto iter = selection_->get_selected();
  if (iter) {
    auto row = *iter;
    signal_selection_changed_.emit(row[model_->columns.start_frame]);
  }
}
