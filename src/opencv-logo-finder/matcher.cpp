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
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "OpenCVLogoFinder.hpp"

using namespace mdl::opencv;


class CsvCallback : public mdl::LogoFinderCallback
{
public:
  CsvCallback(const std::string& output_file);
  bool success(const mdl::LogoFinderResult& result) override;
  bool failure(int start_frame) override;

private:
  std::ofstream csv_;
};


int main(int argc, char* argv[])
{
  if (argc < 5) {
    std::cout << "Usage: matcher <video> <output> <start_frame> <frame_interval> [<end_frame>]" << std::endl;
    return 1;
  }

  int start_frame = atoi(argv[3]) - 1;
  int frame_interval = atoi(argv[4]);

  CsvCallback callback(argv[2]);

  OpenCVLogoFinder finder(argv[1], start_frame, frame_interval, callback);
  int end_frame;
  if (argc == 6) {
    end_frame = atof(argv[5]);
  } else {
    end_frame = finder.total_frames();
  }

  std::cout << "Processing video " << argv[1]
            << " from " << start_frame << " until " << end_frame
            << ", interval " << frame_interval
            << ", output " << argv[2]
            << std::endl;

  finder.find_logos();

  std::cout << "Finished" << std::endl;
}


CsvCallback::CsvCallback(const std::string& output_file)
  : csv_(output_file)
{
}


bool CsvCallback::success(const mdl::LogoFinderResult& result)
{
  std::cout << "Success at " << result.start_frame << std::endl;
  csv_ << result.start_frame << ';'
       << result.x << ';' << result.y << ';'
       << result.width << ';' << result.height
       << std::endl;
  return true;
}


bool CsvCallback::failure(int start_frame)
{
  std::cout << "Failure at " << start_frame << std::endl;
  return true;
}
