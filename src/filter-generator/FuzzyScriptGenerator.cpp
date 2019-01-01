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
#include <functional>
#include <random>
#include <chrono>
#include <algorithm>
#include <ostream>

#include <boost/algorithm/clamp.hpp>
#include <boost/optional.hpp>

#include "FuzzyScriptGenerator.hpp"
#include "Filters.hpp"
#include "FilterList.hpp"

using namespace fg;


FuzzyScriptGenerator::FuzzyScriptGenerator(const FilterList& filter_list,
                                           int frame_width, int frame_height, double fps,
                                           double fuzzyness)
  : RegularScriptGenerator(filter_list, frame_width, frame_height, fps)
{
  double mean = (fuzzyness - 1) / 2;
  double stddev = mean / 2;
  std::default_random_engine generator(std::chrono::system_clock::now().time_since_epoch().count());
  std::normal_distribution<double> distribution(mean, stddev);
  rng = std::bind(distribution, generator);
}


std::shared_ptr<FuzzyScriptGenerator> FuzzyScriptGenerator::create(const FilterList& filter_list,
                                                                   int frame_width, int frame_height, double fps,
                                                                   double fuzzyness)
{
  return std::shared_ptr<FuzzyScriptGenerator>(new FuzzyScriptGenerator(filter_list, frame_width, frame_height, fps, fuzzyness));
}


void FuzzyScriptGenerator::generate_ffmpeg_script(std::ostream& out) const
{
  auto& first_filter = *(filter_list_.begin());
  first_filter_start_ = first_filter.first - 1;
  RegularScriptGenerator::generate_ffmpeg_script(out);
}


std::string FuzzyScriptGenerator::get_enable_expression(int start_frame, maybe_int next_start_frame) const
{
  if (next_start_frame) {
    length_ = *next_start_frame - start_frame;
  }

  int adjusted_start = adjust_start(start_frame);
  maybe_int adjusted_next = adjust_end(next_start_frame);
  return RegularScriptGenerator::get_enable_expression(adjusted_start, adjusted_next);
}


int FuzzyScriptGenerator::adjust_start(int start_frame) const
{
  int new_start = start_frame - rng()*length_;
  return boost::algorithm::clamp(new_start, first_filter_start_, start_frame);
}


FuzzyScriptGenerator::maybe_int FuzzyScriptGenerator::adjust_end(maybe_int end_frame) const
{
  if (!end_frame) {
    return boost::none;
  }

  int new_end = *end_frame + rng()*length_;
  return boost::make_optional(std::max(new_end, *end_frame));
}
