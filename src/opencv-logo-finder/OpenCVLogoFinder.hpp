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
    OpenCVLogoFinder(const std::string& file, LogoFinderCallback& callback, bool verbose);

    OpenCVLogoFinder::find_result find_logos() override;

    void stop() override;

  private:
    cv::VideoCapture cap_;
    int total_frames_;

    cv::Mat kernel_sharpen_;
    cv::Mat kernel_gradient_;
    cv::Mat kernel_close_;

    int n_last_failures_;

    bool stop_requested_;

    int current_frame_;

    /**
     * Number of steps to do while searching for the logo in an
     * interval. The first step considers the whole interval, the
     * second divides the interval in to subintervals, the third
     * divides each of the two in two more, for a total of four, and
     * so on. Bigger makes the search slower, but might find more
     * logos.
     */
    int steps_ = 2;
    /**
     * Step between frames used to calculate the average. Bigger makes
     * it faster, but possibly less accurate.
     */
    int frame_step_ = 10;
    /**
     * Number of times to apply CLOSE morphology.
     */
    int close_steps_ = 3;
    /**
     * Threshold for similarity when searching for the logo in extra
     * frames (when the frame interval is not constant). Higher might
     * detect the logo in case the background changes a lot, but can
     * also generate more incorrect results.
     */
    double similarity_threshold_ = 0.7;


    cv::Rect find_logo_in_interval(int interval_start, int interval_end);

    cv::Rect find_boxes(int start_frame, int end_frame);

    void average_frame(int start_frame, int end_frame);
    void go_to_frame(int frame_number);
    void advance_frame();
    void get_frame();

    cv::Rect find_box_in_channel(const cv::Mat& average_frame, int channel);
    cv::Rect select_box(const std::vector<cv::Rect>& boxes);

    int get_logo_transition_point(int current_frame, const cv::Rect& box);

    // Temporary variables
    // They were made class members so that they are allocated only once
    cv::Mat t_avg_;   // Last average frame
    cv::Mat t_frame_; // Last frame read
    // The ones below are used only in one function each
    cv::Mat t_avg_f_;
    cv::Mat t_frame_f_;
    cv::Mat t_sharpened_;
    cv::Mat t_grey_;
    cv::Mat t_gradient_;
    cv::Mat t_thresh_;
    cv::Mat t_closed_;
  };
} }


#endif // MDL_OPENCV_LOGO_FINDER_H
