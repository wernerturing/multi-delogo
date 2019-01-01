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
#ifndef MDL_OPENCV_INTERVAL_CALCULATOR_H
#define MDL_OPENCV_INTERVAL_CALCULATOR_H

#include <vector>
#include <utility>


namespace mdl { namespace opencv {
  class IntervalCalculator
  {
  public:
    static std::vector<std::pair<int, int>> get_subintervals(int interval_start, int interval_end, int n_subintervals);

  private:
    static void adjust_last_subinterval(std::vector<std::pair<int, int>>& subintervals, int interval_end);
  };
} }


#endif // MDL_OPENCV_INTERVAL_CALCULATOR_H
