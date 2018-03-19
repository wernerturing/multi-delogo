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
#include <stdexcept>

#include <boost/algorithm/string.hpp>

#include "Filters.hpp"
#include "Exceptions.hpp"

using namespace fg;


Filter::~Filter()
{
}


bool Filter::affects_audio() const
{
  return false;
}


NullFilter* NullFilter::load(const std::string& parameters)
{
  if (parameters != "") {
    throw InvalidParametersException();
  }

  return new NullFilter();
}


FilterType NullFilter::type() const
{
  return FilterType::NO_OP;
}


std::string NullFilter::name() const
{
  return "none";
}


std::string NullFilter::save_str() const
{
  return "none;";
}


std::string NullFilter::ffmpeg_str(const std::string& between_expr) const
{
  return "";
}


RectangularFilter::RectangularFilter(int x, int y, int width, int height)
  : x_(x), y_(y), width_(width), height_(height)
{
}


void RectangularFilter::load_rectangle(const std::string& parameters,
                                       int& x, int& y, int& width, int& height)
{
  std::vector<std::string> dimensions;
  boost::split(dimensions, parameters, boost::is_any_of(";"));
  if (dimensions.size() != 4) {
    throw InvalidParametersException();
  }

  try {
    x      = std::stoi(dimensions[0]);
    y      = std::stoi(dimensions[1]);
    width  = std::stoi(dimensions[2]);
    height = std::stoi(dimensions[3]);
  } catch (std::invalid_argument& e) {
    throw InvalidParametersException();
  }
}


int RectangularFilter::x() const
{
  return x_;
}


int RectangularFilter::y() const
{
  return y_;
}


int RectangularFilter::width() const
{
  return width_;
}


int RectangularFilter::height() const
{
  return height_;
}


std::string RectangularFilter::rectangle_save_str() const
{
  std::string buf;
  buf.append(std::to_string(x_)).push_back(';');
  buf.append(std::to_string(y_)).push_back(';');
  buf.append(std::to_string(width_)).push_back(';');
  buf.append(std::to_string(height_));
  return buf;
}


std::string RectangularFilter::rectangle_ffmpeg_str() const
{
  std::string buf;
  buf.append("x=").append(std::to_string(x_)).push_back(':');
  buf.append("y=").append(std::to_string(y_)).push_back(':');
  buf.append("w=").append(std::to_string(width_)).push_back(':');
  buf.append("h=").append(std::to_string(height_));
  return buf;
}


DelogoFilter::DelogoFilter(int x, int y, int width, int height)
  : RectangularFilter(x, y, width, height)
{
}


DelogoFilter* DelogoFilter::load(const std::string& parameters)
{
  int x, y, width, height;

  load_rectangle(parameters, x, y, width, height);
  return new DelogoFilter(x, y, width, height);
}


FilterType DelogoFilter::type() const
{
  return FilterType::DELOGO;
}


std::string DelogoFilter::name() const
{
  return "delogo";
}


std::string DelogoFilter::save_str() const
{
  std::string buf("delogo;");
  buf.append(rectangle_save_str());
  return buf;
}


std::string DelogoFilter::ffmpeg_str(const std::string& between_expr) const
{
  std::string buf("delogo=");
  buf.append(between_expr).push_back(':');
  buf.append(rectangle_ffmpeg_str());
  return buf;
}


DrawboxFilter::DrawboxFilter(int x, int y, int width, int height)
  : RectangularFilter(x, y, width, height)
{
}


DrawboxFilter* DrawboxFilter::load(const std::string& parameters)
{
  int x, y, width, height;

  load_rectangle(parameters, x, y, width, height);
  return new DrawboxFilter(x, y, width, height);
}


FilterType DrawboxFilter::type() const
{
  return FilterType::DRAWBOX;
}


std::string DrawboxFilter::name() const
{
  return "drawbox";
}


std::string DrawboxFilter::save_str() const
{
  std::string buf("drawbox;");
  buf.append(rectangle_save_str());
  return buf;
}


std::string DrawboxFilter::ffmpeg_str(const std::string& between_expr) const
{
  std::string buf("drawbox=");
  buf.append(between_expr).push_back(':');
  buf.append(rectangle_ffmpeg_str());
  buf.append(":c=black:t=max");
  return buf;
}


CutFilter* CutFilter::load(const std::string& parameters)
{
  if (parameters != "") {
    throw InvalidParametersException();
  }

  return new CutFilter();
}


FilterType CutFilter::type() const
{
  return FilterType::CUT;
}


std::string CutFilter::name() const
{
  return "cut";
}


bool CutFilter::affects_audio() const
{
  return true;
}


std::string CutFilter::save_str() const
{
  return "cut;";
}


std::string CutFilter::ffmpeg_str(const std::string& between_expr) const
{
  return "";
}


ReviewFilter* ReviewFilter::load(const std::string& parameters)
{
  if (parameters != "") {
    throw InvalidParametersException();
  }

  return new ReviewFilter();
}


FilterType ReviewFilter::type() const
{
  return FilterType::REVIEW;
}


std::string ReviewFilter::name() const
{
  return "review";
}


std::string ReviewFilter::save_str() const
{
  return "review;";
}


std::string ReviewFilter::ffmpeg_str(const std::string& between_expr) const
{
  return "";
}
