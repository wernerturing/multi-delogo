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
#include <algorithm>

#include "FrameNavigatorUtil.hpp"

int mdl::get_zoom_to_fit_ratio(int image_width, int image_height, int window_width, int window_height)
{
  double ratio_width = (double) window_width / image_width;
  double ratio_height = (double) window_height / image_height;

  int ratio = std::min(ratio_width, ratio_height) * 100;
  if (ratio > 100) {
    ratio = 100;
  }

  return ratio;
}
