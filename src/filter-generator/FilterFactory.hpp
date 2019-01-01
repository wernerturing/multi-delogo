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
#ifndef FG_FILTER_FACTORY_H
#define FG_FILTER_FACTORY_H

#include <string>

#include "Filters.hpp"

namespace fg {
  class FilterFactory
  {
  public:
    static filter_ptr load(const std::string& serialized);
    static filter_ptr create(FilterType type);
    static filter_ptr create(FilterType type, int x, int y, int width, int height);
    static filter_ptr convert(filter_ptr original, FilterType new_type);

  private:
    static filter_ptr load(const std::string& type, const std::string& parameters);

    static bool is_no_parameters(FilterType type);
    static bool is_rectangular(FilterType type);
  };
}

#endif // FG_FILTER_FACTORY_H
