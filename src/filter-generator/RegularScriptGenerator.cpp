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
#include <memory>
#include <string>
#include <ostream>

#include "RegularScriptGenerator.hpp"
#include "Filters.hpp"
#include "FilterList.hpp"

using namespace fg;


RegularScriptGenerator::RegularScriptGenerator(const FilterList& filter_list)
  : filter_list_(filter_list)
  , first_filter_(true)
{
}


std::shared_ptr<RegularScriptGenerator> RegularScriptGenerator::create(const FilterList& filter_list)
{
  return std::shared_ptr<RegularScriptGenerator>(new RegularScriptGenerator(filter_list));
}


void RegularScriptGenerator::generate_ffmpeg_script(std::ostream& out) const
{
  if (filter_list_.empty()) {
    return;
  }

  generate_ffmpeg_script_intermediary_filters(out);
  generate_ffmpeg_script_last_filter(out);
}


void RegularScriptGenerator::generate_ffmpeg_script_intermediary_filters(std::ostream& out) const
{
  FilterList::const_iterator i = filter_list_.begin();

  while (true) {
    auto& current = *i++;
    if (i == filter_list_.end()) {
      break;
    }

    auto& next = *i;

    std::string frame_expr = get_frame_expression(current.first - 1,
                                                  next.first - 1);
    std::string ffmpeg_str = current.second->ffmpeg_str(frame_expr);
    if (ffmpeg_str != "") {
      if (first_filter_) {
        first_filter_ = false;
      } else {
        out << ",\n";
      }
      out << ffmpeg_str;
    }
  }
}


void RegularScriptGenerator::generate_ffmpeg_script_last_filter(std::ostream& out) const
{
  auto& last = *--filter_list_.end();
  std::string frame_expr = get_frame_expression(last.first - 1);
  std::string ffmpeg_str = last.second->ffmpeg_str(frame_expr);
  if (ffmpeg_str != "") {
    if (!first_filter_) {
      out << ",\n";
    }
    out << ffmpeg_str;
  }
}


std::string RegularScriptGenerator::get_frame_expression(int start_frame, int next_start_frame) const
{
  return "enable='between(n," + std::to_string(start_frame)
    + ',' + std::to_string(next_start_frame - 1)
    + ")\'";
}


std::string RegularScriptGenerator::get_frame_expression(int start_frame) const
{
  return "enable='gte(n," + std::to_string(start_frame) + ")'";
}
