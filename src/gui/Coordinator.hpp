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
#ifndef MDL_COORDINATOR_H
#define MDL_COORDINATOR_H

#include <gtkmm.h>

#include "FilterList.hpp"
#include "FilterListModel.hpp"
#include "FrameNavigator.hpp"


namespace mdl {
  class Coordinator
  {
  public:
    Coordinator(FilterList& filter_list, FrameNavigator& frame_navigator);

  private:
    FilterList& filter_list_;
    Glib::RefPtr<FilterListModel> filter_model_;
    FrameNavigator& frame_navigator_;

    void on_filter_selected(int start_frame);

    sigc::connection on_frame_changed_;
    void on_frame_changed(int frame);
  };
}

#endif // MDL_COORDINATOR_H
