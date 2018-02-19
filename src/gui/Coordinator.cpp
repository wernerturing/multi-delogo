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

#include "Coordinator.hpp"
#include "FilterList.hpp"
#include "FrameNavigator.hpp"
#include "FilterPanelFactory.hpp"

using namespace mdl;


Coordinator::Coordinator(FilterList& filter_list,
                         FrameNavigator& frame_navigator,
                         int frame_width, int frame_height)
  : filter_list_(filter_list)
  , filter_model_(filter_list.get_model())
  , frame_navigator_(frame_navigator)
  , panel_factory_(frame_width, frame_height)
  , current_filter_(nullptr)
{
  on_filter_selected_ = filter_list_.signal_selection_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_filter_selected));

  on_frame_changed_ = frame_navigator_.signal_frame_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_frame_changed));

  frame_navigator_.change_displayed_frame(1);
}


void Coordinator::on_filter_selected(int start_frame)
{
  on_frame_changed_.block();
  frame_navigator_.change_displayed_frame(start_frame);
  on_frame_changed_.block(false);

  auto iter = filter_model_->get_for_frame(start_frame);
  change_filter(iter);
}


void Coordinator::on_frame_changed(int frame)
{
  auto iter = filter_model_->get_for_frame(frame);

  on_filter_selected_.block();
  if (iter && (*iter)[filter_model_->columns.start_frame] == frame) {
    filter_list_.select(iter);
  } else {
    filter_list_.unselect();
  }
  on_filter_selected_.block(false);

  change_filter(iter);
}


void Coordinator::change_filter(const FilterListModel::iterator& iter)
{
  if (!iter) {
    return;
  }

  fg::Filter* filter = (*iter)[filter_model_->columns.filter];
  if (filter == current_filter_) {
    return;
  }

  FilterPanel* panel = Gtk::manage(panel_factory_.create(filter));
  filter_list_.set_filter_panel(panel);
  current_filter_ = filter;
}
