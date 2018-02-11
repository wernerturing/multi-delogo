# Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
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

bin_PROGRAMS = multi-delogo

multi_delogo_SOURCES = main.cpp \
                       MultiDelogoApp.cpp \
                       NumericEntry.cpp \
                       FrameView.cpp \
                       MovieWindow.cpp

noinst_HEADERS = common/Exceptions.hpp \
                 common/FrameProvider.hpp \
                 MultiDelogoApp.hpp \
                 NumericEntry.hpp \
                 FrameView.hpp \
                 MovieWindow.hpp


localedir = $(datadir)/locale

multi_delogo_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" \
                        $(GTKMM_CFLAGS)

frame_provider_lib = ../opencv-frame-provider/libopencv-frame-provider.a $(OPENCV_LIBS)

multi_delogo_LDADD = $(frame_provider_lib) $(GTKMM_LIBS)
