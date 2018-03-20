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

noinst_LIBRARIES = libopencv-logo-finder.a \
                   libfilter-list-logo-adapter.a


libopencv_logo_finder_a_SOURCES = OpenCVLogoFinder.cpp \
                                  OpenCVLogoFinder.hpp \
                                  IntervalCalculator.cpp \
                                  IntervalCalculator.hpp

libopencv_logo_finder_a_CPPFLAGS = -I.. $(OPENCV_CFLAGS)


libfilter_list_logo_adapter_a_SOURCES = FilterListAdapter.cpp \
                                        FilterListAdapter.hpp \
                                        LogoFinderFactory.cpp

libfilter_list_logo_adapter_a_CPPFLAGS = -I..


noinst_PROGRAMS = matcher

matcher_SOURCES = matcher.cpp

matcher_CPPFLAGS = -I..

matcher_LDADD = libfilter-list-logo-adapter.a \
                libopencv-logo-finder.a \
                libfilter-list-logo-adapter.a \
                ../filter-generator/libfilter-generator.a \
                $(OPENCV_LIBS)
