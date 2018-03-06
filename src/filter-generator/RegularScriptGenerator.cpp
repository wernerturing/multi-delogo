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
#include <utility>
#include <ostream>
#include <algorithm>

#include <boost/algorithm/string/join.hpp>
#include <boost/optional.hpp>

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


bool RegularScriptGenerator::affects_audio() const
{
  return std::any_of(filter_list_.begin(), filter_list_.end(),
                     [](auto& f) { return f.second->affects_audio(); });
}


void RegularScriptGenerator::generate_ffmpeg_script(std::ostream& out) const
{
  if (filter_list_.empty()) {
    return;
  }

  out << "[0:v]\n";
  generate_ffmpeg_script_standard_filters(out);
  generate_ffmpeg_script_cuts(out);
  out << "\n[out_v]";
}


void RegularScriptGenerator::generate_ffmpeg_script_standard_filters(std::ostream& out) const
{
  FilterList::const_iterator i = filter_list_.begin();

  while (i != filter_list_.end()) {
    auto& current = *i++;

    fg::Filter* filter = current.second;

    int start = current.first - 1;
    maybe_int next_start;
    if (i != filter_list_.end()) {
      auto& next = *i;
      next_start = boost::make_optional(next.first - 1);
    }

    if (filter->type() == fg::FilterType::CUT) {
      process_cut_filter(start, next_start);
    } else {
      process_standard_filter(filter, start, next_start, out);
    }
  }
}


void RegularScriptGenerator::process_standard_filter(fg::Filter* filter,
                                                     int start_frame, maybe_int next_start_frame,
                                                     std::ostream& out) const
{
  std::string frame_expr = get_enable_expression(start_frame, next_start_frame);
  std::string ffmpeg_str = filter->ffmpeg_str(frame_expr);
  if (ffmpeg_str != "") {
    out << separator() << ffmpeg_str;
  }
}


void RegularScriptGenerator::process_cut_filter(int start_frame, maybe_int next_start_frame) const
{
  cuts_.push_back(std::make_pair(start_frame, next_start_frame));
}


void RegularScriptGenerator::generate_ffmpeg_script_cuts(std::ostream& out) const
{
  if (cuts_.empty()) {
    return;
  }

  std::vector<std::string> positions;
  positions.resize(cuts_.size());
  std::transform(cuts_.begin(), cuts_.end(), positions.begin(),
                 [this](auto& i) { return get_frame_expression(i.first, i.second); });

  std::string expressions(boost::algorithm::join(positions, "+"));
  out << separator() << "select='not(" << expressions << ")',setpts=N/FRAME_RATE/TB";
}


std::string RegularScriptGenerator::separator() const
{
  if (first_filter_) {
    first_filter_ = false;
    return "";
  } else {
    return ",\n";
  }
}


std::string RegularScriptGenerator::get_enable_expression(int start_frame, maybe_int next_start_frame) const
{
  return "enable='" + get_frame_expression(start_frame, next_start_frame) + "'";
}


std::string RegularScriptGenerator::get_frame_expression(int start_frame, maybe_int next_start_frame) const
{
  if (next_start_frame) {
    return "between(n," + std::to_string(start_frame)
      + ',' + std::to_string(*next_start_frame - 1) + ")";
  } else {
    return "gte(n," + std::to_string(start_frame) + ")";
  }
}
