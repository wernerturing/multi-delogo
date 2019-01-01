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
#include <cstdlib>
#include <limits>
#include <memory>
#include <iostream>
#include <fstream>

#include "filter-generator/FilterData.hpp"

#include "FilterListAdapter.hpp"

using namespace mdl;


class MatcherCallback : public LogoFinderCallback
{
public:
  MatcherCallback(int frame_interval);
  void success(const LogoFinderResult& result) override;
  void failure(int start_frame, int end_frame) override;
  void set_end_frame(int end_frame);
  void set_finder(LogoFinder* finder);

private:
  int frame_interval_;
  int end_frame_;
  LogoFinder* finder_;
};


int main(int argc, char* argv[])
{
  if (argc < 5) {
    std::cout << "Usage: logo-finder <video> <output> <start_frame> <frame_interval_min> <frame_interval_max> [<end_frame>]" << std::endl;
    return 1;
  }

  int start_frame = atoi(argv[3]) - 1;
  int frame_interval_min = atoi(argv[4]);
  int frame_interval_max = atoi(argv[5]);

  fg::FilterData filter_data;
  filter_data.set_movie_file(argv[1]);
  filter_data.set_jump_size(frame_interval_min);

  MatcherCallback matcher_callback(frame_interval_min);

  std::shared_ptr<LogoFinder> finder
    = create_logo_finder(filter_data, matcher_callback, true);
  finder->set_start_frame(start_frame);
  finder->set_frame_interval_min(frame_interval_min);
  finder->set_extra_frames(frame_interval_max - frame_interval_min);

  int end_frame;
  if (argc == 7) {
    end_frame = atoi(argv[6]);
  } else {
    end_frame = std::numeric_limits<int>::max();
  }

  matcher_callback.set_end_frame(end_frame);
  matcher_callback.set_finder(finder.get());

  std::cout << "Processing video " << argv[1]
            << " from " << start_frame << " until " << end_frame
            << ", interval " << frame_interval_min << "-" << frame_interval_max
            << ", output " << argv[2]
            << std::endl;

  auto res = finder->find_logos();

  std::ofstream output(argv[2]);
  filter_data.save(output);

  if (res.first) {
    std::cout << "Finished successfully" << std::endl;
  } else {
    std::cout << "Error: " << res.second << std::endl;
    return 2;
  }
}


MatcherCallback::MatcherCallback(int frame_interval)
  : frame_interval_(frame_interval)
{
}


void MatcherCallback::success(const LogoFinderResult& result)
{
  std::cout << "Success at "
            << result.start_frame << "-" << result.end_frame << std::endl;
  if ((result.start_frame + frame_interval_) > end_frame_) {
    finder_->stop();
  }
}


void MatcherCallback::failure(int start_frame, int end_frame)
{
  std::cout << "Failure at "
            << start_frame << "-" << end_frame << std::endl;
  if ((start_frame + frame_interval_) > end_frame_) {
    finder_->stop();
  }
}


void MatcherCallback::set_end_frame(int end_frame)
{
  end_frame_ = end_frame;
}


void MatcherCallback::set_finder(LogoFinder* finder)
{
  finder_ = finder;
}
