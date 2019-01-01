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
#include <vector>
#include <utility>

#include "IntervalCalculator.hpp"

using namespace mdl::opencv;


#define BOOST_TEST_MODULE finder
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(get_subintervals)

BOOST_AUTO_TEST_CASE(should_return_one_subinterval_for_first_level)
{
  auto subintervals = IntervalCalculator::get_subintervals(1000, 1500, 1);

  BOOST_REQUIRE(subintervals.size() == 1);
  BOOST_TEST(subintervals[0].first == 1000);
  BOOST_TEST(subintervals[0].second == 1500);
}


BOOST_AUTO_TEST_CASE(should_return_one_subinterval_for_second_level)
{
  auto subintervals = IntervalCalculator::get_subintervals(1000, 1455, 2);

  BOOST_REQUIRE(subintervals.size() == 2);

  BOOST_TEST(subintervals[0].first == 1000);
  BOOST_TEST(subintervals[0].second == 1228);

  BOOST_TEST(subintervals[1].first == 1228);
  BOOST_TEST(subintervals[1].second == 1455);
}


BOOST_AUTO_TEST_CASE(should_return_one_subinterval_for_third_level)
{
  auto subintervals = IntervalCalculator::get_subintervals(723, 1324, 4);

  BOOST_REQUIRE(subintervals.size() == 4);

  BOOST_TEST(subintervals[0].first == 723);
  BOOST_TEST(subintervals[0].second == 874);

  BOOST_TEST(subintervals[1].first == 874);
  BOOST_TEST(subintervals[1].second == 1025);

  BOOST_TEST(subintervals[2].first == 1025);
  BOOST_TEST(subintervals[2].second == 1176);

  BOOST_TEST(subintervals[3].first == 1176);
  BOOST_TEST(subintervals[3].second == 1324);
}

BOOST_AUTO_TEST_SUITE_END()
