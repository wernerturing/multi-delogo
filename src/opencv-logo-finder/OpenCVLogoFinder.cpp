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
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>

#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>

#include "gui/common/Exceptions.hpp"

#include "OpenCVLogoFinder.hpp"
#include "IntervalCalculator.hpp"

using namespace mdl::opencv;


OpenCVLogoFinder::OpenCVLogoFinder(const std::string& file, LogoFinderCallback& callback)
  : LogoFinder(callback)
  , n_last_failures_(0)
  , stop_requested_(false)
{
  cap_.open(file);
  if (!cap_.isOpened()) {
    throw mdl::VideoNotOpenedException();
  }
  total_frames_ = cap_.get(cv::CAP_PROP_FRAME_COUNT);

  kernel_sharpen_ = cv::Mat(3, 3, CV_64F, cv::Scalar(1));
  kernel_sharpen_.at<double>(1, 1) = -7;

  kernel_morphology_ = cv::Mat::ones(3, 3, CV_8U);

  kernel_close_ = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 1));

  int width_ = cap_.get(cv::CAP_PROP_FRAME_WIDTH);
  int height_ = cap_.get(cv::CAP_PROP_FRAME_HEIGHT);
  t_avg_f_.create(height_, width_, CV_64FC3);
}


void OpenCVLogoFinder::find_logos()
{
  int interval_start = start_frame_;
  while (interval_start < total_frames_) {
    int interval_end = interval_start + frame_interval_min_;
    if (interval_end > total_frames_) {
      interval_end = total_frames_;
    }

    std::cout << "find_logos iteration for [" << interval_start
              << ", " << interval_end << ")" << std::endl;
    cv::Rect box = find_logo_in_interval(interval_start, interval_end);
    std::cout << "  logo found = ["
              << box.x << " " << box.y << " "
              << box.width << " " << box.height << "]\n";

    if (stop_requested_) {
      break;
    }

    if (box.x != 0) {
      int new_start = get_logo_transition_point(interval_end, box);

      LogoFinderResult result{.start_frame = interval_start,
                              .end_frame = new_start - 1,
                              .x = box.x, .y = box.y,
                              .width = box.width, .height = box.height};
      callback_.success(result);

      interval_start = new_start;
      n_last_failures_ = 0;
    } else {
      callback_.failure(interval_start, interval_end - 1);

      interval_start += frame_interval_min_;
      ++n_last_failures_;
    }
  }
}


cv::Rect OpenCVLogoFinder::find_logo_in_interval(int interval_start, int interval_end)
{
  int n_subintervals = 1;
  int level = 1;
  while (level <= steps_) {
    auto subintervals = IntervalCalculator::get_subintervals(interval_start, interval_end, n_subintervals);

    std::vector<cv::Rect> subinterval_boxes;
    for (const auto& subinterval: subintervals) {
      subinterval_boxes.push_back(find_boxes(subinterval.first, subinterval.second));
    }

    cv::Rect interval_box = select_box(subinterval_boxes);
    if (interval_box.x > 0) {
      return interval_box;
    }

    // Subdivide each interval in two and try again
    n_subintervals *= 2;
    ++level;

    if (stop_requested_) {
      break;
    }
  }

  // Not found
  return cv::Rect();
}


cv::Rect OpenCVLogoFinder::find_boxes(int start_frame, int end_frame)
{
  std::cout << "  find_boxes in [" << start_frame
            << ", " << end_frame << ")\n";
  average_frame(start_frame, end_frame);

  cv::filter2D(t_avg_, t_sharpened_, -1, kernel_sharpen_);

  std::vector<cv::Rect> boxes;
  for (int channel = 0; channel <= 2; ++channel) {
    boxes.push_back(find_box_in_channel(t_sharpened_, channel));
  }

  return select_box(boxes);
}


void OpenCVLogoFinder::average_frame(int start_frame, int end_frame)
{
  t_avg_f_.setTo(cv::Scalar(0, 0, 0));

  go_to_frame(start_frame);
  int frames = 0;
  for (int f = start_frame; f < end_frame; ++f) {
    get_next_frame();
    if (f % frame_step_ != 0) {
      continue;
    }

    t_frame_.convertTo(t_frame_f_, CV_64FC3);
    t_avg_f_ += t_frame_f_;
    ++frames;

    if (stop_requested_) {
      break;
    }
  }

  t_avg_f_.convertTo(t_avg_, CV_8U, 1. / frames);
}


void OpenCVLogoFinder::go_to_frame(int frame_number)
{
  cap_.set(cv::CAP_PROP_POS_FRAMES, frame_number);
}


void OpenCVLogoFinder::get_next_frame()
{
  bool success = cap_.read(t_frame_);
  if (!success) {
    throw mdl::FrameNotAvailableException();
  }
}


cv::Rect OpenCVLogoFinder::find_box_in_channel(const cv::Mat& average_frame, int channel)
{
  cv::extractChannel(average_frame, t_grey_, channel);
  cv::morphologyEx(t_grey_, t_gradient_, cv::MORPH_GRADIENT, kernel_morphology_);
  cv::threshold(t_gradient_, t_thresh_, 190, 255, cv::THRESH_BINARY);
  cv::morphologyEx(t_thresh_, t_closed_, cv::MORPH_CLOSE, kernel_close_, cv::Point(-1, -1), close_steps_);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(t_closed_, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

  for (auto& contour: contours) {
    cv::Rect rect = cv::boundingRect(contour);
    if ((rect.width >= min_logo_width_ && rect.width <= max_logo_width_)
        && (rect.height >= min_logo_height_ && rect.height <= max_logo_height_)) {
      return rect;
    }
  }

  return cv::Rect(0, 0, 0, 0);
}


cv::Rect OpenCVLogoFinder::select_box(const std::vector<cv::Rect>& boxes)
{
  std::vector<cv::Rect> non_zero;
  std::remove_copy_if(boxes.begin(), boxes.end(), std::back_inserter(non_zero),
    [](const auto& box) {
      return box.x == 0;
    });

  if (non_zero.empty()) {
    return boxes[0];
  }

  return *std::min_element(non_zero.begin(), non_zero.end(),
    [](const auto& box1, const auto& box2) {
      return box1.x < box2.x;
    });
}


int OpenCVLogoFinder::get_logo_transition_point(int current_frame, const cv::Rect& box)
{
  if (current_frame >= total_frames_) {
    return current_frame;
  }

  int extra_frames_to_check = extra_frames_ + n_last_failures_*extra_frames_;
  if (extra_frames_to_check <= 0) {
    return current_frame;
  }

  for (int i = 0; i < extra_frames_to_check; ++i) {
    cv::Mat logo = cv::Mat(t_frame_, box).clone();
    get_next_frame();
    cv::Mat logo_next = cv::Mat(t_frame_, box);

    double n = cv::norm(logo, logo_next, cv::NORM_L2);
    double similarity = n / (logo.rows * logo.cols);

    if (similarity > similarity_threshold_) {
      break;
    }

    ++current_frame;
  }

  return current_frame;
}


void OpenCVLogoFinder::stop()
{
  stop_requested_ = true;
}
