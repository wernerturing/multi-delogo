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
#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "Coordinator.hpp"
#include "FilterList.hpp"
#include "FrameNavigator.hpp"
#include "FrameView.hpp"
#include "FilterPanelFactory.hpp"

using namespace mdl;


Coordinator::Coordinator(FilterList& filter_list,
                         FrameNavigator& frame_navigator,
                         int frame_width, int frame_height)
  : filter_list_(filter_list)
  , filter_model_(filter_list.get_model())
  , frame_navigator_(frame_navigator)
  , frame_view_(frame_navigator_.get_frame_view())
  , panel_factory_(frame_navigator_.get_number_of_frames(),
                   frame_width, frame_height)
  , current_filter_panel_(nullptr)
  , current_filter_(nullptr)
  , scroll_filter_(true)
{
  on_filter_selected_ = filter_list_.signal_selection_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_filter_selected));

  filter_list_.signal_previous_filter().connect(
    sigc::mem_fun(*this, &Coordinator::on_previous_filter));
  filter_list_.signal_next_filter().connect(
    sigc::mem_fun(*this, &Coordinator::on_next_filter));

  filter_list_.signal_remove_filter().connect(
    sigc::mem_fun(*this, &Coordinator::on_remove_filter));

  on_filter_type_changed_ = filter_list_.signal_type_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_filter_type_changed));

  frame_navigator_.signal_frame_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_frame_changed));

  on_frame_rectangle_changed_ = frame_view_.signal_rectangle_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_frame_rectangle_changed));

  frame_navigator_.change_displayed_frame(1);
}


void Coordinator::on_previous_filter()
{
  auto iter = filter_model_->get_for_frame(current_frame_ - 1);
  if (!iter) {
    return;
  }

  frame_navigator_.change_displayed_frame((*iter)[filter_model_->columns.start_frame]);
}


void Coordinator::on_next_filter()
{
  auto iter = filter_model_->get_for_frame(current_frame_);
  if (iter) {
    ++iter;
  } else {
    iter = filter_model_->children().begin();
  }

  if (!iter) {
    return;
  }

  frame_navigator_.change_displayed_frame((*iter)[filter_model_->columns.start_frame]);
}


void Coordinator::set_scroll_filter(bool state)
{
  scroll_filter_ = state;
}


int Coordinator::get_current_frame()
{
  return current_frame_;
}


void Coordinator::on_filter_selected(int start_frame)
{
  frame_navigator_.change_displayed_frame(start_frame);
}


void Coordinator::on_frame_changed(int new_frame)
{
  update_current_filter_if_necessary();

  auto iter = filter_model_->get_for_frame(new_frame);

  if (iter && (*iter)[filter_model_->columns.start_frame] == new_frame) {
    select_row(iter);
  } else {
    unselect_rows();
  }

  change_displayed_filter(iter);

  current_frame_ = new_frame;
}


void Coordinator::select_row(const FilterListModel::iterator& iter)
{
  on_filter_selected_.block();
  filter_list_.select(iter);
  on_filter_selected_.block(false);

  filter_list_.scroll_to_row(iter);
}


void Coordinator::unselect_rows()
{
  on_filter_selected_.block();
  filter_list_.unselect();
  on_filter_selected_.block(false);
}


void Coordinator::change_displayed_filter(const FilterListModel::iterator& iter)
{
  if (!iter) {
    return;
  }

  int start_frame = (*iter)[filter_model_->columns.start_frame];
  fg::Filter* filter = (*iter)[filter_model_->columns.filter];
  if (filter == current_filter_) {
    return;
  }
  current_filter_ = filter;

  update_displayed_panel(filter->type(), panel_factory_.create(start_frame, filter));

  auto rect = current_filter_panel_->get_rectangle();
  if (rect) {
    frame_view_.show_rectangle(*rect);
    if (scroll_filter_) {
      frame_view_.scroll_to_current_rectangle();
    }
  } else {
    frame_view_.hide_rectangle();
  }
}


void Coordinator::update_displayed_panel(fg::FilterType type, FilterPanel* panel)
{
  current_filter_panel_ = Gtk::manage(panel);

  on_filter_type_changed_.block();
  filter_list_.set_filter(type, current_filter_panel_);
  on_filter_type_changed_.block(false);

  on_panel_rectangle_changed_ = current_filter_panel_->signal_rectangle_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_panel_rectangle_changed));
}


void Coordinator::on_filter_type_changed(fg::FilterType new_type)
{
  if (!current_filter_) {
    return;
  }

  update_current_filter_if_necessary();
  add_new_filter_if_not_on_filter_starting_frame(true);

  FilterPanel* new_panel = panel_factory_.convert(current_frame_, current_filter_, new_type);
  update_displayed_panel(new_type, new_panel);
  update_current_filter(true);
}


void Coordinator::on_frame_rectangle_changed(Rectangle rect)
{
  if (!current_filter_panel_) {
    create_new_filter_panel();
  }

  on_panel_rectangle_changed_.block();
  current_filter_panel_->set_rectangle(rect);
  on_panel_rectangle_changed_.block(false);

  add_new_filter_if_not_on_filter_starting_frame();
}


void Coordinator::on_panel_rectangle_changed(Rectangle rect)
{
  on_frame_rectangle_changed_.block();
  frame_view_.show_rectangle(rect);
  on_frame_rectangle_changed_.block(false);

  add_new_filter_if_not_on_filter_starting_frame();
}


void Coordinator::create_new_filter_panel()
{
  fg::FilterType filter_type = filter_list_.get_selected_type();
  update_displayed_panel(filter_type, panel_factory_.create(current_frame_, filter_type));
}


void Coordinator::update_current_filter_if_necessary()
{
  update_current_filter(false);
}

void Coordinator::update_current_filter(bool force_update)
{
  auto iter = filter_model_->get_by_start_frame(current_frame_);
  if (!force_update
      && (!iter || !current_filter_panel_ || !current_filter_panel_->is_changed())) {
    return;
  }

  auto new_filter = current_filter_panel_->get_filter();
  (*iter)[filter_model_->columns.filter] = new_filter;
  current_filter_ = new_filter;
}


void Coordinator::add_new_filter_if_not_on_filter_starting_frame(bool always_add)
{
  auto iter = filter_model_->get_by_start_frame(current_frame_);
  if (iter) {
    return;
  }

  if (!always_add && !current_filter_panel_->creates_filter()) {
    return;
  }

  current_filter_ = current_filter_panel_->get_filter();
  auto inserted_row = filter_model_->insert(current_frame_, current_filter_);
  current_filter_panel_->set_changed(false);
  current_filter_panel_->set_start_frame(current_frame_);

  select_row(inserted_row);
}


void Coordinator::on_remove_filter()
{
  auto iter = filter_model_->get_by_start_frame(current_frame_);
  on_filter_selected_.block();
  filter_model_->remove(iter);
  on_filter_selected_.block(false);
  on_frame_changed(current_frame_);
}
