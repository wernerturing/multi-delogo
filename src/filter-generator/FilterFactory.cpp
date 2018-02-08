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

#include "Filters.hpp"
#include "FilterFactory.hpp"
#include "Exceptions.hpp"

using namespace fg;


Filter* FilterFactory::load(const std::string& serialized)
{
  auto pos = serialized.find_first_of(';');
  if (pos == std::string::npos) {
    throw InvalidFilterException();
  }

  std::string type = serialized.substr(0, pos);
  std::string parameters = serialized.substr(pos + 1);

  return create(type, parameters);
}


Filter* FilterFactory::create(const std::string& type, const std::string& parameters)
{
  if (type == "none") {
    return NullFilter::load(parameters);
  } else if (type == "delogo") {
    return DelogoFilter::load(parameters);
  } else if (type == "drawbox") {
    return DrawboxFilter::load(parameters);
  } else {
    throw UnknownFilterException();
  }
}
