dnl Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
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

AC_INIT([multi-delogo], [2.6.0])
AC_CONFIG_SRCDIR([src/gui/main.cpp])

AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign dist-xz no-dist-gzip])
AM_SILENT_RULES([yes])

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_17([noext], [mandatory])

AC_PROG_RANLIB

AX_PTHREAD

PKG_CHECK_MODULES([GLIB], [glib-2.0 >= 2.70.0])

PKG_CHECK_MODULES([GTKMM], [gtkmm-4.0 >= 4.10])

dnl Hides the deprecated APIs from the headers, so that any use of them
dnl becomes a compilation error. Added to GTKMM_CFLAGS because every
dnl directory that compiles against gtkmm uses it.
AC_ARG_ENABLE([no-deprecated],
  AS_HELP_STRING([--enable-no-deprecated],
                 [Make the use of deprecated gtkmm APIs a compilation error]),
  [],
  [enable_no_deprecated=no])
AS_IF([test "x$enable_no_deprecated" = xyes],
  [
    GTKMM_CFLAGS="$GTKMM_CFLAGS -DGTKMM_DISABLE_DEPRECATED -DGDKMM_DISABLE_DEPRECATED -DPANGOMM_DISABLE_DEPRECATED -DGIOMM_DISABLE_DEPRECATED -DGLIBMM_DISABLE_DEPRECATED"
  ])

AC_SUBST([GTKMM_CFLAGS])
AC_SUBST([GTKMM_LIBS])

WT_OPENCV([OPENCV], [4.0], [opencv_videoio opencv_imgproc opencv_core])
AC_SUBST([OPENCV_CFLAGS])
AC_SUBST([OPENCV_LIBS])

AX_BOOST_BASE([1.46], [], [
  AC_MSG_ERROR([boost library could not be found])])
case $host in
  *mingw*)
  ;;
  *)
    AX_BOOST_UNIT_TEST_FRAMEWORK
  ;;
esac
AM_CONDITIONAL([RUN_TESTS], [test x$BOOST_UNIT_TEST_FRAMEWORK_LIB != x])

GETTEXT_PACKAGE=multi-delogo
AC_SUBST([GETTEXT_PACKAGE])
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
                   [The domain to use with gettext])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.19.8])

AC_SUBST([WINDOWS_LDFLAGS])
case $host in
  *mingw*)
    AC_ARG_ENABLE([windows-console],
      AS_HELP_STRING([--enable-windows-console], [Enable console window]),
      [],
      [
        WINDOWS_LDFLAGS="-mwindows"
      ])
  ;;
esac

AC_CONFIG_FILES([Makefile
                 src/Makefile
                 src/filter-generator/Makefile
                 src/opencv-frame-provider/Makefile
                 src/opencv-logo-finder/Makefile
                 src/gui/Makefile
                 test/Makefile
                 test/filter-generator/Makefile
                 test/opencv-logo-finder/Makefile
                 test/gui/Makefile
                 po/Makefile.in
                 docs/Makefile])
AC_OUTPUT
