/*
 * Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
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
#include <iostream>
#include <sstream>
#include <iomanip>

#include "ScriptGenerator.hpp"

using namespace fg;


ScriptGenerator::ScriptGenerator(double fps, bool no_audio)
  : fps_(fps)
  , no_audio_(no_audio)
{
  fps_str_ = make_fps_str(fps);
}


std::string ScriptGenerator::make_fps_str(double fps)
{
  std::ostringstream ss;
  ss.imbue(std::locale::classic());
  ss << std::fixed << std::setprecision(6);
  ss << fps;

  return ss.str();
}


double ScriptGenerator::fps()
{
  return fps_;
}


std::string ScriptGenerator::fps_str()
{
  return fps_str_;
}


bool ScriptGenerator::no_audio()
{
  return no_audio_;
}
