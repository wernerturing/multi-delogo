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
#include <string>
#include <memory>

#include <glibmm/refptr.h>

#include <opencv2/videoio.hpp>

#include "gui/common/Exceptions.hpp"
#include "gui/common/FrameProvider.hpp"

#include "OpenCVFrameProvider.hpp"


Glib::RefPtr<mdl::FrameProvider> mdl::create_frame_provider(const std::string& movie_filename)
{
  std::unique_ptr<cv::VideoCapture> video(new cv::VideoCapture(movie_filename));
  if (!video->isOpened()) {
    throw mdl::VideoNotOpenedException();
  }

  return Glib::RefPtr<mdl::FrameProvider>(new mdl::opencv::OpenCVFrameProvider(std::move(video)));
}
