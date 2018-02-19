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

using namespace mdl;


Coordinator::Coordinator(FilterList& filter_list, FrameNavigator& frame_navigator)
  : filter_list_(filter_list)
  , filter_model_(filter_list.get_model())
  , frame_navigator_(frame_navigator)
{
  filter_list_.signal_selection_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_filter_selected));

  on_frame_changed_ = frame_navigator_.signal_frame_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_frame_changed));
}


void Coordinator::on_filter_selected(int start_frame)
{
  printf("filter starting at %d selected\n", start_frame);
  on_frame_changed_.block();
  frame_navigator_.change_displayed_frame(start_frame);
  on_frame_changed_.block(false);
}


void Coordinator::on_frame_changed(int frame)
{
  printf("frame changed to %d\n", frame);
  auto iter = filter_model_->get_for_frame(frame);
  if (iter && (*iter)[filter_model_->columns.start_frame] == frame) {
    filter_list_.select(iter);
  } else {
    filter_list_.unselect();
  }
}
