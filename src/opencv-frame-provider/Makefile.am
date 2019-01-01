# Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>
#
# This file is part of multi-delogo.
#
# multi-delogo is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# multi-delogo is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.

noinst_LIBRARIES = libopencv-frame-provider.a

libopencv_frame_provider_a_SOURCES = OpenCVFrameProvider.cpp \
                                     OpenCVFrameProviderFactory.cpp

noinst_HEADERS = OpenCVFrameProvider.hpp

libopencv_frame_provider_a_CPPFLAGS = -I.. $(GTKMM_CFLAGS) $(OPENCV_CFLAGS)
