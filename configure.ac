AC_INIT([multi-delogo], [1.0.0])
AC_CONFIG_SRCDIR([src/gui/main.cpp])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign])

AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11

AC_PROG_RANLIB

PKG_CHECK_MODULES([GTKMM], [gtkmm-3.0 >= 3.4])
AC_SUBST([GTKMM_CFLAGS])
AC_SUBST([GTKMM_LIBS])

PKG_CHECK_MODULES([OPENCV], [opencv >= 3.0])
dnl pkg-config brings all the libs, so override it with only the needed ones
OPENCV_LIBS="-lopencv_videoio -lopencv_imgproc -lopencv_core"
AC_SUBST([OPENCV_CFLAGS])
AC_SUBST([OPENCV_LIBS])

AX_BOOST_BASE([1.46])
AX_BOOST_UNIT_TEST_FRAMEWORK
AM_CONDITIONAL([RUN_TESTS], [test x$BOOST_UNIT_TEST_FRAMEWORK_LIB != x])

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/filter-generator/Makefile
                 src/gui/Makefile
                 test/Makefile
                 test/filter-generator/Makefile])
AC_OUTPUT
