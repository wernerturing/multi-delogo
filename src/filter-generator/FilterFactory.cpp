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
#include <string>
#include <stdexcept>

#include "Filters.hpp"
#include "FilterFactory.hpp"
#include "Exceptions.hpp"

using namespace fg;


filter_ptr FilterFactory::load(const std::string& serialized)
{
  auto pos = serialized.find_first_of(';');
  if (pos == std::string::npos) {
    throw InvalidFilterException();
  }

  std::string type = serialized.substr(0, pos);
  std::string parameters = serialized.substr(pos + 1);

  return load(type, parameters);
}


filter_ptr FilterFactory::load(const std::string& type, const std::string& parameters)
{
  if (type == "none") {
    return NullFilter::load(parameters);
  } else if (type == "delogo") {
    return DelogoFilter::load(parameters);
  } else if (type == "drawbox") {
    return DrawboxFilter::load(parameters);
  } else if (type == "cut") {
    return CutFilter::load(parameters);
  } else if (type == "review") {
    return ReviewFilter::load(parameters);
  } else {
    throw UnknownFilterException();
  }
}


filter_ptr FilterFactory::create(FilterType type)
{
  switch (type) {
  case FilterType::NO_OP:
    return filter_ptr(new NullFilter());

  case FilterType::CUT:
    return filter_ptr(new CutFilter());

  case FilterType::REVIEW:
    return filter_ptr(new ReviewFilter());

  case FilterType::DELOGO:
  case FilterType::DRAWBOX:
    throw InvalidParametersException();

  default:
    throw UnknownFilterException();
  }
}


filter_ptr FilterFactory::create(FilterType type, int x, int y, int width, int height)
{
  if (type == FilterType::DELOGO) {
    return filter_ptr(new DelogoFilter(x, y, width, height));
  } else if (type == FilterType::DRAWBOX) {
    return filter_ptr(new DrawboxFilter(x, y, width, height));
  } else if (is_no_parameters(type)) {
    return create(type);
  }

  throw UnknownFilterException();
}


filter_ptr FilterFactory::convert(filter_ptr original, FilterType new_type)
{
  if (is_rectangular(original->type()) && is_rectangular(new_type)) {
    std::shared_ptr<RectangularFilter> rectangular = std::dynamic_pointer_cast<RectangularFilter>(original);
    filter_ptr converted = create(new_type, rectangular->x(), rectangular->y(), rectangular->width(), rectangular->height());
    return converted;
  }

  if (is_rectangular(new_type)) {
    return create(new_type, 0, 0, 0, 0);
  }

  if (is_no_parameters(new_type) || is_no_parameters(original->type())) {
    return create(new_type);
  }

  throw std::invalid_argument("Unsupported conversion");
}


bool FilterFactory::is_no_parameters(FilterType type)
{
  return type == FilterType::NO_OP
      || type == FilterType::CUT
      || type == FilterType::REVIEW;
}


bool FilterFactory::is_rectangular(FilterType type)
{
  return type == FilterType::DELOGO || type == FilterType::DRAWBOX;
}
