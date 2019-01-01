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
#ifndef FG_FILTER_DATA_H
#define FG_FILTER_DATA_H

#include <string>
#include <istream>
#include <ostream>

#include "FilterList.hpp"


namespace fg {
  class FilterData
  {
  public:
    FilterData();

    void set_movie_file(const std::string& movie_file);
    void set_jump_size(int jump_size);

    std::string movie_file() const;
    int jump_size() const;
    FilterList& filter_list();

    static bool is_filter_data(std::istream& in);
    void load(std::istream& in);
    void save(std::ostream& out) const;

  private:
    const static std::string HEADER_;

    std::string movie_file_;
    int jump_size_;
    FilterList filter_list_;
  };
}

#endif // FG_FILTER_DATA_H
