/*
 * Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
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
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

#include <boost/algorithm/string/join.hpp>
#include <boost/optional.hpp>

#include "RegularScriptGenerator.hpp"
#include "Filters.hpp"
#include "FilterFactory.hpp"
#include "FilterList.hpp"

using namespace fg;


RegularScriptGenerator::RegularScriptGenerator(const FilterList& filter_list,
                                               int frame_width, int frame_height, double fps,
                                               maybe_int scale_width, maybe_int scale_height,
                                               bool no_audio)
  : ScriptGenerator(fps, no_audio)
  , filter_list_(filter_list)
  , frame_width_(frame_width)
  , frame_height_(frame_height)
  , scale_width_(scale_width)
  , scale_height_(scale_height)
  , first_filter_(true)
{
}


std::shared_ptr<RegularScriptGenerator> RegularScriptGenerator::create(const FilterList& filter_list, int frame_width, int frame_height, double fps, maybe_int scale_width, maybe_int scale_height, bool no_audio)
{
  return std::shared_ptr<RegularScriptGenerator>(new RegularScriptGenerator(filter_list, frame_width, frame_height, fps, scale_width, scale_height, no_audio));
}


void RegularScriptGenerator::generate_ffmpeg_script(std::ostream& out) const
{
  if (filter_list_.empty()) {
    return;
  }

  int n_segments = generate_filter_segments(out);
  generate_final_concat(out, n_segments);
}


int RegularScriptGenerator::generate_filter_segments(std::ostream& out) const
{
  int segment = 0;
  FilterList::const_iterator i = filter_list_.begin();
  while (i != filter_list_.end()) {
    auto& current = *i++;
    filter_ptr filter = current.second;

    int start_frame = current.first - 1;
    maybe_int next_start_frame;
    if (i != filter_list_.end()) {
      auto& next = *i;
      next_start_frame = boost::make_optional(next.first - 1);
    }

    if (first_filter_does_not_start_at_first_frame(start_frame)) {
      copy_first_segment_unchanged(out, start_frame);
      segment++;
    }
    first_filter_ = false;

    if (filter->type() == FilterType::CUT) {
      cuts_.push_back(std::make_pair(start_frame, next_start_frame));
      continue;
    }

    generate_segment(out, segment, filter, start_frame, next_start_frame);

    ++segment;
  }

  return segment;
}


bool RegularScriptGenerator::first_filter_does_not_start_at_first_frame(int start_frame) const
{
  return first_filter_ && start_frame != 0;
}


void RegularScriptGenerator::copy_first_segment_unchanged(std::ostream& out, int next_start) const
{
  generate_segment(out, 0, FilterFactory::create(FilterType::NO_OP), 0, next_start);
}


void RegularScriptGenerator::generate_segment(std::ostream& out, int segment, filter_ptr filter,
                                              int start_frame, maybe_int next_start_frame) const
{
  std::string ffmpeg_str = filter->ffmpeg_str(frame_width_, frame_height_);
  out << "[0:v]" << generate_trim(start_frame, next_start_frame) << ",setpts=PTS-STARTPTS";
  if (ffmpeg_str != "") {
    out << "," << ffmpeg_str;
  }
  if (scale_width_) {
    out << ",scale=" << *scale_width_ << ":" << *scale_height_;
  }
  out << "[vs" << segment << "];\n";

  if (!no_audio_) {
    std::string ffmpeg_audio_str = filter->ffmpeg_audio_str();
    out << "[0:a]" << generate_atrim(start_frame, next_start_frame)
        << ",asetpts=PTS-STARTPTS";
    if (ffmpeg_audio_str != "") {
      out << "," << ffmpeg_audio_str;
    }
    out << "[as" << segment << "];\n";
  }
}


std::string RegularScriptGenerator::generate_trim(int start_frame, maybe_int next_start_frame) const
{
  if (next_start_frame) {
    return "trim=start_frame=" + std::to_string(start_frame)
      + ":end_frame=" + std::to_string(*next_start_frame);
  } else {
    return "trim=start_frame=" + std::to_string(start_frame);
  }
}


std::string RegularScriptGenerator::generate_atrim(int start_frame, maybe_int next_start_frame) const
{
  std::stringstream out;
  out << std::fixed << std::setprecision(3);
  double start_time = start_frame/fps_;
  if (next_start_frame) {
    double end_time = *next_start_frame/fps_;
    out << "atrim=start=" << start_time << ":end=" << end_time;
  } else {
    out << "atrim=start=" << start_time;
  }
  return out.str();
}


void RegularScriptGenerator::generate_final_concat(std::ostream& out, int n_segments) const
{
  if (!no_audio_) {
    for (int i = 0; i < n_segments; ++i) {
      out << "[vs" << i << "][as" << i << "]";
    }
    out << "concat=n=" << n_segments << ":v=1:a=1[out_v][out_a]";
  } else {
    for (int i = 0; i < n_segments; ++i) {
      out << "[vs" << i << "]";
    }
    out << "concat=n=" << n_segments << ":v=1:a=0[out_v]";
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
