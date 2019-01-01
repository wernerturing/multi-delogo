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
#ifndef MDL_OPENCV_OPENCV_FRAME_PROVIDER_H
#define MDL_OPENCV_OPENCV_FRAME_PROVIDER_H

#include <string>
#include <memory>

#include <glibmm/objectbase.h>
#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

#include <opencv2/videoio.hpp>

#include "gui/common/FrameProvider.hpp"


namespace mdl { namespace opencv {
  class OpenCVFrameProvider : public FrameProvider
  {
  public:
    OpenCVFrameProvider(std::unique_ptr<cv::VideoCapture> video);

    Glib::RefPtr<Gdk::Pixbuf> get_frame(int frame_number) override;

    int get_frame_width() override;
    int get_frame_height() override;
    int get_number_of_frames() override;
    double get_fps() override;

  private:
    std::unique_ptr<cv::VideoCapture> video_;
    int current_frame_;
    // We keep a structure for the frame data in order to avoid reallocating space every time a frame is requested.
    cv::Mat frame_;
  };
} }


#endif // MDL_OPENCV_OPENCV_FRAME_PROVIDER_H
