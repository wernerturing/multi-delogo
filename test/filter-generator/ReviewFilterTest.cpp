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
#include <memory>
#include <string>

#include "Filters.hpp"
#include "Exceptions.hpp"


#define BOOST_TEST_MODULE null filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_type)
{
  fg::ReviewFilter filter;

  BOOST_CHECK_EQUAL(filter.type(), fg::FilterType::REVIEW);
}


BOOST_AUTO_TEST_CASE(test_name)
{
  fg::ReviewFilter filter;

  BOOST_CHECK_EQUAL(filter.name(), "review");
}


BOOST_AUTO_TEST_CASE(test_save_str)
{
  fg::ReviewFilter filter;

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "review;");
}


BOOST_AUTO_TEST_CASE(test_load)
{
  std::shared_ptr<fg::ReviewFilter> filter = fg::ReviewFilter::load("");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::REVIEW);
}


BOOST_AUTO_TEST_CASE(test_load_with_invalid_parameters)
{
  BOOST_CHECK_THROW(fg::ReviewFilter::load("1;2"), fg::InvalidParametersException);
}
