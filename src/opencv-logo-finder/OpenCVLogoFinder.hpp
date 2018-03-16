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
#ifndef MDL_OPENCV_LOGO_FINDER_H
#define MDL_OPENCV_LOGO_FINDER_H

#include <string>
#include <vector>

#include <opencv2/videoio.hpp>

#include "gui/common/LogoFinder.hpp"


namespace mdl { namespace opencv {
  class OpenCVLogoFinder: public LogoFinder
  {
  public:
    OpenCVLogoFinder(const std::string& file, int start_frame, int frame_interval, LogoFinderCallback& callback);

    int total_frames();

    void find_logos() override;

  private:
    cv::VideoCapture cap_;

    int start_frame_;
    int frame_interval_;

    int width_;
    int height_;
    int total_frames_;

    cv::Mat kernel_sharpen_;

    /**
     * Number of steps to do while searching for the logo in an
     * interval. The first step considers the whole interval, the
     * second divides the interval in to subintervals, the third
     * divides each of the two in two more, for a total of four, and
     * so on. Bigger makes the search slower, but might find more
     * logos.
     */
    int steps_ = 2;

    cv::Rect find_logo_in_interval(int interval_start, int interval_end);

    cv::Rect find_boxes(int start_frame, int end_frame);

    cv::Mat average_frame(int start_frame, int end_frame);
    void go_to_frame(int frame_number);
    cv::Mat get_next_frame();

    cv::Rect find_box_in_channel(const cv::Mat& haystack, int channel);
    cv::Rect select_box(const std::vector<cv::Rect>& boxes);
  };
} }


#endif // MDL_OPENCV_LOGO_FINDER_H
