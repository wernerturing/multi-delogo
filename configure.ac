AC_INIT([multi-delogo], [1.0.0])
AC_CONFIG_SRCDIR([src/gui/main.cpp])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CXX
AC_PROG_RANLIB

PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.4])
AC_SUBST([GTKMM_CFLAGS])
AC_SUBST([GTKMM_LIBS])

PKG_CHECK_MODULES([OPENCV], [opencv >= 3.0])
dnl pkg-config brings all the libs, so override it with only the needed ones
OPENCV_LIBS="-lopencv_videoio -lopencv_imgproc -lopencv_core"
AC_SUBST([OPENCV_CFLAGS])
AC_SUBST([OPENCV_LIBS])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/filter-generator/Makefile
                 src/gui/Makefile])
AC_OUTPUT
