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

#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

#include <opencv2/opencv.hpp>

#include "gui/common/Exceptions.hpp"

#include "OpenCVFrameProvider.hpp"

using namespace mdl::opencv;


OpenCVFrameProvider::OpenCVFrameProvider(std::unique_ptr<cv::VideoCapture> video)
  : FrameProvider()
{
  video_ = std::move(video);
}


Glib::RefPtr<Gdk::Pixbuf> OpenCVFrameProvider::get_frame(int frame_number)
{
  video_->set(CV_CAP_PROP_POS_FRAMES, frame_number);

  cv::Mat bgr_frame;
  bool success = video_->read(bgr_frame);
  if (!success) {
    throw mdl::FrameNotAvailableException();
  }

  cv::cvtColor(bgr_frame, frame_, CV_BGR2RGB);
  return Gdk::Pixbuf::create_from_data(frame_.data,
                                       Gdk::COLORSPACE_RGB,
                                       false, 8,
                                       frame_.cols, frame_.rows,
                                       frame_.step);
}
