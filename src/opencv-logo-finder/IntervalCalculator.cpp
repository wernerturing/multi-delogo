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
#include <vector>
#include <utility>

#include "IntervalCalculator.hpp"

using namespace mdl::opencv;


std::vector<std::pair<int, int>> IntervalCalculator::get_subintervals(int interval_start, int interval_end, int n_subintervals)
{
  int length = ((interval_end - interval_start) / n_subintervals) + 1;

  std::vector<std::pair<int, int>> subintervals;
  for (int i = 0; i < n_subintervals; ++i) {
    subintervals.push_back(std::make_pair(interval_start + i*length,
                                          interval_start + (i+1)*length));
  }

  adjust_last_subinterval(subintervals, interval_end);
  return subintervals;
}


void IntervalCalculator::adjust_last_subinterval(std::vector<std::pair<int, int>>& subintervals, int interval_end)
{
  subintervals.back().second = interval_end;
}
