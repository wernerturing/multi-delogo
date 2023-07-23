dnl Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>
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

AC_DEFUN([WT_OPENCV], [dnl
  PKG_CHECK_MODULES([$1], [opencv >= $2])
  res=""
  for arg in [$]$1_LIBS; do
      case "$arg" in
          -l*)
              for module in [$3]; do
                  case $arg in
                      *$module*)
                          res="$res $arg"
                          ;;
                  esac
              done
              ;;
          *)
              res="$res $arg"
              ;;
      esac
  done
  $1_LIBS=$res
])
