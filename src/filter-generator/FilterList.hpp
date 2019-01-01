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
#ifndef FG_FILTER_LIST_H
#define FG_FILTER_LIST_H

#include <memory>
#include <string>
#include <map>
#include <istream>
#include <ostream>

#include <boost/optional.hpp>

#include "Filters.hpp"


namespace fg {
  class FilterList
  {
  public:
    typedef std::map<int, filter_ptr>::value_type value_type;
    typedef boost::optional<value_type> maybe_type;
    typedef std::map<int, filter_ptr>::size_type size_type;
    typedef std::map<int, filter_ptr>::const_iterator const_iterator;

    FilterList() = default;

    // No copying
    FilterList (const FilterList&) = delete;
    FilterList& operator=(const FilterList&) = delete;

    void insert(int start_frame, filter_ptr filter);
    void remove(int start_frame);
    void change_start_frame(int old_start_frame, int new_start_frame);

    bool empty() const;
    size_type size() const;

    const_iterator begin() const;
    const_iterator end() const;

    maybe_type get_by_start_frame(int start_frame) const;
    maybe_type get_by_position(size_type position) const;
    int get_position(int start_frame) const;
    maybe_type get_filter_for_frame(int frame) const;

    bool has_review_filter() const;

    void load(std::istream& in);
    void save(std::ostream& out) const;


  private:
    std::map<int, filter_ptr> filters_;

    void load_line(const std::string& line);
  };
}

#endif // FG_FILTER_LIST_H
