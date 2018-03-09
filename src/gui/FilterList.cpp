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
  : model_(FilterListModel::create(filter_list))
  , view_(model_)
  , current_panel_(nullptr)
{
  view_.append_column(_("Start frame"), model_->columns.start_frame);
  view_.append_column(_("Filter"), model_->columns.filter_name);

  Gtk::ScrolledWindow* scroll = Gtk::manage(new Gtk::ScrolledWindow());
  scroll->set_shadow_type(Gtk::SHADOW_IN);
  scroll->set_min_content_width(200);
  scroll->set_min_content_height(225);
  scroll->add(view_);

  btn_remove_filter_.set_image_from_icon_name("list-remove");
  btn_remove_filter_.set_tooltip_text(_("Remove the selected filter"));
  btn_remove_filter_.set_sensitive(false);
  Gtk::ButtonBox* buttons = Gtk::manage(new Gtk::ButtonBox());
  buttons->pack_start(btn_remove_filter_, false, false);

  set_orientation(Gtk::ORIENTATION_VERTICAL);
  set_row_spacing(8);
  add(*scroll);
  add(*buttons);
  add(filter_type_);

  selection_ = view_.get_selection();
  selection_->signal_changed().connect(sigc::mem_fun(*this, &FilterList::on_selection_changed));

  btn_remove_filter_.signal_clicked().connect(
    sigc::mem_fun(signal_remove_filter_, &type_signal_remove_filter::emit));

  filter_type_.signal_type_changed().connect(
    sigc::mem_fun(signal_type_changed_, &type_signal_type_changed::emit));
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


void FilterList::scroll_to_row(const Gtk::TreeModel::iterator& iter)
{
  view_.scroll_to_row(model_->get_path(iter));
}


void FilterList::set_filter(fg::FilterType filter_type, Gtk::Widget* panel)
{
  filter_type_.set(filter_type);

  if (current_panel_) {
    remove(*current_panel_);
  }

  add(*panel);
  show_all();
  current_panel_ = panel;
}


fg::FilterType FilterList::get_selected_type() const
{
  return filter_type_.get();
}


FilterList::type_signal_selection_changed FilterList::signal_selection_changed()
{
  return signal_selection_changed_;
}


FilterList::type_signal_remove_filter FilterList::signal_remove_filter()
{
  return signal_remove_filter_;
}


FilterList::type_signal_type_changed FilterList::signal_type_changed()
{
  return signal_type_changed_;
}


void FilterList::on_selection_changed()
{
  auto iter = selection_->get_selected();
  if (iter) {
    btn_remove_filter_.set_sensitive(true);
    auto row = *iter;
    signal_selection_changed_.emit(row[model_->columns.start_frame]);
  } else {
    btn_remove_filter_.set_sensitive(false);
  }
}
