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
#include <memory>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "FilterList.hpp"
#include "FilterListModel.hpp"

#include "filter-generator/FilterList.hpp"

using namespace mdl;


FilterList::FilterList(BaseObjectType* cobject,
                       const Glib::RefPtr<Gtk::Builder>& builder,
                       fg::FilterList& filter_list)
  : Gtk::Grid(cobject)
  , model_(FilterListModel::create(filter_list))
  , view_(nullptr)
  , filter_type_(nullptr)
  , current_panel_(nullptr)
{
  builder->get_widget("filter_view", view_);
  view_->set_model(model_);
  view_->append_column(_("Start frame"), model_->columns.start_frame);
  view_->append_column(_("Filter"), model_->columns.filter_name);

  configure_buttons(builder);

  builder->get_widget_derived("filter_type", filter_type_);

  selection_ = view_->get_selection();
  selection_->signal_changed().connect(sigc::mem_fun(*this, &FilterList::on_selection_changed));

  filter_type_->signal_type_changed().connect(
    sigc::mem_fun(signal_type_changed_, &type_signal_type_changed::emit));
}


void FilterList::configure_buttons(const Glib::RefPtr<Gtk::Builder>& builder)
{
  Gtk::Button* btn_prev_filter = nullptr;
  builder->get_widget("btn_prev_filter", btn_prev_filter);
  btn_prev_filter->signal_clicked().connect(
    sigc::mem_fun(signal_previous_filter_, &type_signal_button::emit));

  builder->get_widget("btn_remove_filter", btn_remove_filter_);
  btn_remove_filter_->signal_clicked().connect(
    sigc::mem_fun(signal_remove_filter_, &type_signal_button::emit));

  Gtk::Button* btn_next_filter = nullptr;
  builder->get_widget("btn_next_filter", btn_next_filter);
  btn_next_filter->signal_clicked().connect(
    sigc::mem_fun(signal_next_filter_, &type_signal_button::emit));

  Gtk::Button* btn_shift = nullptr;
  builder->get_widget("btn_shift", btn_shift);
  btn_shift->signal_clicked().connect(
    sigc::mem_fun(signal_shift_, &type_signal_button::emit));
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


Gtk::TreeModel::iterator FilterList::get_selected() const
{
  return selection_->get_selected();
}


void FilterList::scroll_to_row(const Gtk::TreeModel::iterator& iter)
{
  view_->scroll_to_row(model_->get_path(iter));
}


void FilterList::set_filter(fg::FilterType filter_type, Gtk::Widget* panel)
{
  filter_type_->set(filter_type);

  if (current_panel_) {
    remove(*current_panel_);
  }

  add(*panel);
  show_all();
  current_panel_ = panel;
}


fg::FilterType FilterList::get_selected_type() const
{
  return filter_type_->get();
}


void FilterList::refresh_list()
{
  view_->unset_model();
  view_->set_model(model_);
}


FilterList::type_signal_selection_changed FilterList::signal_selection_changed()
{
  return signal_selection_changed_;
}


FilterList::type_signal_button FilterList::signal_previous_filter()
{
  return signal_previous_filter_;
}


FilterList::type_signal_button FilterList::signal_next_filter()
{
  return signal_next_filter_;
}


FilterList::type_signal_button FilterList::signal_remove_filter()
{
  return signal_remove_filter_;
}


FilterList::type_signal_button FilterList::signal_shift()
{
  return signal_shift_;
}


FilterList::type_signal_type_changed FilterList::signal_type_changed()
{
  return signal_type_changed_;
}


void FilterList::on_selection_changed()
{
  auto iter = selection_->get_selected();
  if (iter) {
    btn_remove_filter_->set_sensitive(true);
    auto row = *iter;
    signal_selection_changed_.emit(row[model_->columns.start_frame]);
  } else {
    btn_remove_filter_->set_sensitive(false);
  }
}
