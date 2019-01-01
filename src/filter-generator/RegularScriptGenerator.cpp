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
#include <memory>
#include <string>
#include <utility>
#include <ostream>
#include <algorithm>
#include <numeric>
#include <clocale>

#include <boost/algorithm/string/join.hpp>
#include <boost/optional.hpp>

#include "RegularScriptGenerator.hpp"
#include "Filters.hpp"
#include "FilterList.hpp"

using namespace fg;


RegularScriptGenerator::RegularScriptGenerator(const FilterList& filter_list, int frame_width, int frame_height, double fps)
  : filter_list_(filter_list)
  , frame_width_(frame_width)
  , frame_height_(frame_height)
  , first_filter_(true)
{
  fps_ = make_fps_str(fps);
}


std::string RegularScriptGenerator::make_fps_str(double fps)
{
  char* original_locale = setlocale(LC_NUMERIC, nullptr);
  setlocale(LC_NUMERIC, "C");
  std::string result = "/" + std::to_string(fps);
  setlocale(LC_NUMERIC, original_locale);
  return result;
}



std::shared_ptr<RegularScriptGenerator> RegularScriptGenerator::create(const FilterList& filter_list, int frame_width, int frame_height, double fps)
{
  return std::shared_ptr<RegularScriptGenerator>(new RegularScriptGenerator(filter_list, frame_width, frame_height, fps));
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
  generate_ffmpeg_script_audio(out);
}


void RegularScriptGenerator::generate_ffmpeg_script_standard_filters(std::ostream& out) const
{
  FilterList::const_iterator i = filter_list_.begin();

  while (i != filter_list_.end()) {
    auto& current = *i++;

    filter_ptr filter = current.second;

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


void RegularScriptGenerator::process_standard_filter(filter_ptr filter,
                                                     int start_frame, maybe_int next_start_frame,
                                                     std::ostream& out) const
{
  std::string frame_expr = get_enable_expression(start_frame, next_start_frame);
  std::string ffmpeg_str = filter->ffmpeg_str(frame_expr, frame_width_, frame_height_);
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


void RegularScriptGenerator::generate_ffmpeg_script_audio(std::ostream& out) const
{
  if (cuts_.empty()) {
    return;
  }

  std::vector<std::string> positions;
  positions.resize(cuts_.size());
  std::transform(cuts_.begin(), cuts_.end(), positions.begin(),
                 [this](auto& i) { return get_audio_expression(i.first, i.second); });

  std::string expressions(boost::algorithm::join(positions, "+"));
  out << ";\n[0:a]aselect='not(" << expressions << ")',asetpts=N/SR/TB[out_a]";
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


std::string RegularScriptGenerator::get_audio_expression(int start_frame, maybe_int next_start_frame) const
{
  if (next_start_frame) {
    return "between(t,"
      + std::to_string(start_frame) + fps_
      + ',' + std::to_string(*next_start_frame - 1) + fps_
      + ")";
  } else {
    return "gte(t,"
      + std::to_string(start_frame) + fps_
      + ")";
  }
}


int RegularScriptGenerator::resulting_frames(int original_frames) const
{
  int cut_frames = std::accumulate(cuts_.begin(), cuts_.end(), 0,
    [original_frames](int sum, std::pair<int, maybe_int>& i) {
      return sum + (i.second.value_or(original_frames) - i.first);
    });

  return original_frames - cut_frames;
}
