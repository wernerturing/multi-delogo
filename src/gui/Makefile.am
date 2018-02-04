bin_PROGRAMS = multi-delogo

multi_delogo_SOURCES = main.cpp \
                       NumericEntry.cpp \
                       MovieWindow.cpp

noinst_HEADERS = common/Exceptions.hpp \
                 common/FrameProvider.hpp \
                 NumericEntry.hpp \
                 MovieWindow.hpp


multi_delogo_CPPFLAGS = $(GTKMM_CFLAGS)

frame_provider_lib = ../opencv-frame-provider/libopencv-frame-provider.a $(OPENCV_LIBS)

multi_delogo_LDADD = $(frame_provider_lib) $(GTKMM_LIBS)
