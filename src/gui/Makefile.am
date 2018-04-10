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
                       MultiDelogoAppWindow.cpp \
                       NumericEntry.cpp \
                       ETRProgressBar.cpp \
                       FrameView.cpp \
                       FrameNavigator.cpp \
                       FrameNavigatorUtil.cpp \
                       FilterListModel.cpp \
                       FilterPanels.cpp \
                       FilterPanelFactory.cpp \
                       FilterType.cpp \
                       FilterList.cpp \
                       EditAction.cpp \
                       UndoManager.cpp \
                       Coordinator.cpp \
                       MovieWindow.cpp \
                       FindLogosWindow.cpp \
                       FFmpegExecutor.cpp \
                       EncodeWindow.cpp \
                       Utils.cpp \
                       InitialWindow.cpp

noinst_HEADERS = common/Exceptions.hpp \
                 common/Rectangle.hpp \
                 common/FrameProvider.hpp \
                 common/LogoFinder.hpp \
                 MultiDelogoApp.hpp \
                 MultiDelogoAppWindow.hpp \
                 NumericEntry.hpp \
                 ETRProgressBar.hpp \
                 FrameView.hpp \
                 FrameNavigator.hpp \
                 FrameNavigatorUtil.hpp \
                 FilterListModel.hpp \
                 FilterPanels.hpp \
                 FilterPanelFactory.hpp \
                 FilterType.hpp \
                 FilterList.hpp \
                 EditAction.hpp \
                 UndoManager.hpp \
                 Coordinator.hpp \
                 MovieWindow.hpp \
                 FindLogosWindow.hpp \
                 FFmpegExecutor.hpp \
                 EncodeWindow.hpp \
                 Utils.hpp \
                 InitialWindow.hpp


localedir = $(datadir)/locale

multi_delogo_CPPFLAGS = -DLOCALEDIR=\"$(localedir)\" \
                        -I.. \
                        $(PTHREAD_CFLAGS) \
                        $(GTKMM_CFLAGS) \
                        $(GOOCANVASMM_CFLAGS)

frame_provider_lib = ../opencv-frame-provider/libopencv-frame-provider.a $(OPENCV_LIBS)

multi_delogo_LDADD = ../filter-generator/libfilter-generator.a \
                     ../opencv-logo-finder/libfilter-list-logo-adapter.a \
                     ../opencv-logo-finder/libopencv-logo-finder.a \
                     $(frame_provider_lib) \
                     $(PTHREAD_CFLAGS) $(PTHREAD_LIBS) \
                     $(GTKMM_LIBS) \
                     $(GOOCANVASMM_LIBS) \
                     $(WINDOWS_LDFLAGS)
