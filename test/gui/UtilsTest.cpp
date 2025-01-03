/*
 * Copyright (C) 2018-2025 Werner Turing <werner.turing@protonmail.com>
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

#include "Utils.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE utils
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(format_time_tests)

BOOST_AUTO_TEST_CASE(should_format_time_1)
{
  long min = 6;
  long sec = 8;
  long ms = 321;
  std::string result = format_time(min*60*1000 + sec*1000 + ms);

  BOOST_CHECK_EQUAL(result, "06:08.321");
}

BOOST_AUTO_TEST_CASE(should_format_time_2)
{
  long min = 12;
  long sec = 9;
  long ms = 27;
  std::string result = format_time(min*60*1000 + sec*1000 + ms);

  BOOST_CHECK_EQUAL(result, "12:09.027");
}

BOOST_AUTO_TEST_CASE(should_format_time_3)
{
  long min = 61;
  long sec = 23;
  long ms = 100;
  std::string result = format_time(min*60*1000 + sec*1000 + ms);

  BOOST_CHECK_EQUAL(result, "61:23.100");
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(format_time_with_hour_tests)

BOOST_AUTO_TEST_CASE(should_format_time_with_hour_1)
{
  long min = 6;
  long sec = 8;
  long ms = 321;
  std::string result = format_time_with_hour(min*60*1000 + sec*1000 + ms);

  BOOST_CHECK_EQUAL(result, "0:06:08.321");
}

BOOST_AUTO_TEST_CASE(should_format_time_2)
{
  long hour = 1;
  long min = 2;
  long sec = 23;
  long ms = 100;
  std::string result
    = format_time_with_hour(hour*60*60*1000 + min*60*1000 + sec*1000 + ms);

  BOOST_CHECK_EQUAL(result, "1:02:23.100");
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(format_time_based_on_total_tests)

BOOST_AUTO_TEST_CASE(should_not_include_hours_if_total_is_less_than_1_hour)
{
  long min = 6;
  long sec = 8;
  long ms = 321;
  long time = min*60*1000 + sec*1000 + ms;
  long total = 59*60*1000 + 59*1000 + 999;

  std::string result = format_time_based_on_total(time, total);

  BOOST_CHECK_EQUAL(result, "06:08.321");
}

BOOST_AUTO_TEST_CASE(should_include_hours_if_total_is_1_hour_or_more)
{
  long min = 6;
  long sec = 8;
  long ms = 321;
  long time = min*60*1000 + sec*1000 + ms;
  long total = 60*60*1000;

  std::string result = format_time_based_on_total(time, total);

  BOOST_CHECK_EQUAL(result, "0:06:08.321");
}

BOOST_AUTO_TEST_SUITE_END()
