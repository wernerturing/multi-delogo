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
#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <boost/version.hpp>

#if BOOST_VERSION >= 106400
#include <ostream>

namespace fg {
  std::ostream& boost_test_print_type(std::ostream& ostr, FilterType const& type)
  {
    switch (type) {
    case FilterType::NO_OP:
      ostr << "NO_OP";
      break;

    case FilterType::DELOGO:
      ostr << "DELOGO";
      break;

    case FilterType::DRAWBOX:
      ostr << "DRAWBOX";
      break;

    case FilterType::CUT:
      ostr << "CUT";
      break;

    case FilterType::REVIEW:
      ostr << "REVIEW";
      break;
    }

    return ostr;
  }
}

#else

BOOST_TEST_DONT_PRINT_LOG_VALUE(fg::FilterType)

#endif

#endif // TEST_HELPERS_H
