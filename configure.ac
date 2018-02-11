dnl Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
dnl
dnl This file is part of multi-delogo.
dnl
dnl multi-delogo is free software: you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation, either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl multi-delogo is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl You should have received a copy of the GNU General Public License
dnl along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.

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

PKG_CHECK_MODULES([GOOCANVASMM], [goocanvasmm-2.0])
AC_SUBST([GOOCANVASMM_CFLAGS])
AC_SUBST([GOOCANVASMM_LIBS])

PKG_CHECK_MODULES([OPENCV], [opencv >= 3.0])
dnl pkg-config brings all the libs, so override it with only the needed ones
OPENCV_LIBS="-lopencv_videoio -lopencv_imgproc -lopencv_core"
AC_SUBST([OPENCV_CFLAGS])
AC_SUBST([OPENCV_LIBS])

AX_BOOST_BASE([1.46], [], [
  AC_MSG_ERROR([boost library could not be found])])
AX_BOOST_UNIT_TEST_FRAMEWORK
AM_CONDITIONAL([RUN_TESTS], [test x$BOOST_UNIT_TEST_FRAMEWORK_LIB != x])

IT_PROG_INTLTOOL([0.35.0])
GETTEXT_PACKAGE=multi-delogo
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [The domain to use with gettext])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])


AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/filter-generator/Makefile
                 src/opencv-frame-provider/Makefile
                 src/gui/Makefile
                 test/Makefile
                 test/filter-generator/Makefile
                 test/gui/Makefile
                 po/Makefile.in])
AC_OUTPUT
