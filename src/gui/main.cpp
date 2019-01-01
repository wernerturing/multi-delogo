/*
 * Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>
 *
 * This file is part of multi-delogo.
 *
 * multi-delogo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * multi-delogo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifdef __MINGW32__
#  include <windows.h>
#endif

#include <glibmm.h>

#include <libintl.h>

#include "MultiDelogoApp.hpp"


int main(int argc, char *argv[])
{
#ifndef __MINGW32__
  bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
#else
  char file[2048];
  GetModuleFileName(nullptr, file, 2048 - 1);
  std::string dir = Glib::path_get_dirname(file);
  dir.append("/locale");
  bindtextdomain(GETTEXT_PACKAGE, dir.c_str());
#endif
  bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
  textdomain(GETTEXT_PACKAGE);

#ifdef __MINGW32__ // Use normal window borders
  Glib::setenv("GTK_CSD", "0");
#endif

  auto app = mdl::MultiDelogoApp::create();
  return app->run(argc, argv);
}
