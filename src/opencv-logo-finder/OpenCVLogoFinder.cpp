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

#include "OpenCVLogoFinder.hpp"

using namespace mdl::opencv;


OpenCVLogoFinder::OpenCVLogoFinder(const std::string& file, int start_frame, int frame_interval, LogoFinderCallback& callback)
  : LogoFinder(callback)
  , cap_(file)
  , start_frame_(start_frame)
  , frame_interval_(frame_interval)
{
  // TODO: check cap_

  width_ = cap_.get(cv::CAP_PROP_FRAME_WIDTH);
  height_ = cap_.get(cv::CAP_PROP_FRAME_HEIGHT);

  total_frames_ = cap_.get(cv::CAP_PROP_FRAME_COUNT);

  kernel_sharpen_ = cv::Mat(3, 3, CV_64F, cv::Scalar(1));
  kernel_sharpen_.at<double>(1, 1) = -7;
}


int OpenCVLogoFinder::total_frames()
{
  return total_frames_;
}


void OpenCVLogoFinder::find_logos()
{
  for (int interval_start = start_frame_;
       interval_start < total_frames_;
       interval_start += frame_interval_) {
    int interval_end = interval_start + frame_interval_;
    if (interval_end > total_frames_) {
      interval_end = total_frames_;
    }

    std::cout << "find_logos iteration for [" << interval_start
              << ", " << interval_end << ")" << std::endl;
    cv::Rect box = find_logo_in_interval(interval_start, interval_end);
    std::cout << "  logo found = ["
              << box.x << " " << box.y << " "
              << box.width << " " << box.height << "]\n";

    bool cont;
    if (box.x != 0) {
      LogoFinderResult result{.start_frame = interval_start,
                              .x = box.x, .y = box.y,
                              .width = box.width, .height = box.height};
      cont = callback_.success(result);
    } else {
      cont = callback_.failure(interval_start);
    }

    if (!cont) {
      break;
    }
  }
}


cv::Rect OpenCVLogoFinder::find_logo_in_interval(int interval_start, int interval_end)
{
  int n_subintervals = 1;
  int level = 1;
  while (level <= steps_) {
    auto subintervals = get_subintervals(interval_start, interval_end, n_subintervals);

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
  }

  // Not found
  return cv::Rect();
}


std::vector<std::pair<int, int>> OpenCVLogoFinder::get_subintervals(int interval_start, int interval_end, int n_subintervals)
{
  int length = ((interval_end - interval_start) / n_subintervals) + 1;

  std::vector<std::pair<int, int>> subintervals;
  for (int i = 0; i < n_subintervals; ++i) {
    subintervals.push_back(std::make_pair(interval_start + i*length,
                                          interval_start + (i+1)*length));
  }

  adjust_last_subinterval(subintervals, interval_end);
  return subintervals;
}


void OpenCVLogoFinder::adjust_last_subinterval(std::vector<std::pair<int, int>>& subintervals, int interval_end)
{
  subintervals.back().second = interval_end;
}


cv::Rect OpenCVLogoFinder::find_boxes(int start_frame, int end_frame)
{
  std::cout << "  find_boxes in [" << start_frame
            << ", " << end_frame << ")\n";
  cv::Mat avg = average_frame(start_frame, end_frame);

  cv::Mat sharpened;
  cv::filter2D(avg, sharpened, -1, kernel_sharpen_);

  std::vector<cv::Rect> boxes;
  for (int channel = 0; channel <= 2; ++channel) {
    boxes.push_back(find_box_in_channel(sharpened, channel));
  }

  return select_box(boxes);
}


cv::Mat OpenCVLogoFinder::average_frame(int start_frame, int end_frame)
{
  cv::Mat avg(height_, width_, CV_64FC3);
  avg.setTo(cv::Scalar(0, 0, 0));

  cv::Mat temp;
  int frames = 0;
  for (int f = start_frame; f < end_frame; f += frame_step_) {
    get_frame(f).convertTo(temp, CV_64FC3);
    avg += temp;
    ++frames;
  }

  avg.convertTo(avg, CV_8U, 1. / frames);
  return avg;
}


cv::Mat OpenCVLogoFinder::get_frame(int frame_number)
{
  cap_.set(cv::CAP_PROP_POS_FRAMES, frame_number);
  cv::Mat frame;
  cap_.read(frame);
  // TODO: Check success of read
  return frame;
}


cv::Rect OpenCVLogoFinder::find_box_in_channel(const cv::Mat& average_frame, int channel)
{
  cv::Mat grey;
  cv::extractChannel(average_frame, grey, channel);

  cv::Mat kernel = cv::Mat::ones(3, 3, CV_8U);
  cv::morphologyEx(grey, grey, cv::MORPH_GRADIENT, kernel);

  cv::Mat thresh;
  cv::threshold(grey, thresh, 190, 255, cv::THRESH_BINARY);

  kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(7, 1));
  cv::Mat dilated;
  cv::dilate(thresh, dilated, kernel, cv::Point(-1, -1), 5);

  std::vector<std::vector<cv::Point>> contours;
  cv::findContours(dilated, contours, cv::RETR_CCOMP, cv::CHAIN_APPROX_NONE);

  for (auto& contour: contours) {
    cv::Rect rect = cv::boundingRect(contour);
    if ((rect.height > 8 && rect.height < 24) && (rect.width > 60 && rect.width < 160)) {
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
