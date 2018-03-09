/*
 * Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
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
#include <string>
#include <iomanip>

#include <glibmm.h>
#include <glibmm/i18n.h>

#include "EncodeWindowUtil.hpp"
#include "FFmpegExecutor.hpp"


std::string mdl::get_progress_str(const FFmpegExecutor::Progress& progress)
{
  return Glib::ustring::compose(_("%1%% done, %2"),
                                (int) (progress.percentage * 100),
                                get_time_remaining(progress));
}


std::string mdl::get_time_remaining(const FFmpegExecutor::Progress& progress)
{
  std::string min_str = Glib::ustring::format(std::setfill(L'0'), std::setw(2),
                                              progress.minutes_remaining);
  std::string sec_str = Glib::ustring::format(std::setfill(L'0'), std::setw(2),
                                              progress.seconds_remaining);
  return Glib::ustring::compose(_("about %1:%2:%3 left"),
                                progress.hours_remaining, min_str, sec_str);
}
