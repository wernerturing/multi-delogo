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
#ifndef MDL_ENCODE_WINDOW_UTIL_H
#define MDL_ENCODE_WINDOW_UTIL_H

#include <string>

#include "FFmpegExecutor.hpp"


namespace mdl {
  std::string get_progress_str(const FFmpegExecutor::Progress& progress);
  std::string get_time_remaining(const FFmpegExecutor::Progress& progress);
}

#endif // MDL_ENCODE_WINDOW_UTIL_H
