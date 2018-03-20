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
#ifndef MDL_FIND_LOGOS_WINDOW_H
#define MDL_FIND_LOGOS_WINDOW_H

#include <memory>

#include <gtkmm.h>

#include "filter-generator/FilterData.hpp"

#include "MultiDelogoAppWindow.hpp"


namespace mdl {
  class FindLogosWindow : public MultiDelogoAppWindow
  {
  public:
    FindLogosWindow(fg::FilterData& filter_data, int total_frames);

  private:
    fg::FilterData& filter_data_;
    int total_frames_;


    Gtk::Grid* create_buttons();
  };
}

#endif // MDL_FIND_LOGOS_WINDOW_H
