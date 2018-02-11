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

AUTOMAKE_OPTIONS = subdir-objects

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I../../src -I../../src/gui $(GTKMM_CFLAGS)
LDADD =  ../../src/filter-generator/libfilter-generator.a \
         $(GTKMM_LIBS) \
         $(BOOST_UNIT_TEST_FRAMEWORK_LIB)


check_PROGRAMS = TestSelectionRect

TestSelectionRect_SOURCES = TestSelectionRect.cpp \
                            ../../src/gui/FrameView.cpp
TestSelectionRect_CPPFLAGS = $(AM_CPPFLAGS) $(GOOCANVASMM_CFLAGS)
TestSelectionRect_LDADD = $(LDADD) $(GOOCANVASMM_LIBS)

TESTS = $(check_PROGRAMS)
