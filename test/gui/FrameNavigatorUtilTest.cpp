/*
 * Copyright (C) 2018-2024 Werner Turing <werner.turing@protonmail.com>
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
#include "FrameNavigatorUtil.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE FrameNavigator utils
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_SUITE(get_zoom_to_fit_ratio_tests,
                      * boost::unit_test::tolerance(0.001))

BOOST_AUTO_TEST_CASE(should_fit_the_width)
{
  double ratio = get_zoom_to_fit_ratio(1920, 1080, 800, 800);
  BOOST_TEST(ratio == 0.416146);
}


BOOST_AUTO_TEST_CASE(should_fit_the_height)
{
  double ratio = get_zoom_to_fit_ratio(1280, 720, 800, 200);
  BOOST_TEST(ratio == 0.276389);
}


BOOST_AUTO_TEST_CASE(should_return_1_if_image_fits_window)
{
  double ratio = get_zoom_to_fit_ratio(640, 480, 800, 800);
  BOOST_TEST(ratio == 1);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(calculate_position_tests)

BOOST_AUTO_TEST_CASE(should_calculate_position_1)
{
  BOOST_CHECK_EQUAL(calculate_position(0, 29.97), 0);
  BOOST_CHECK_EQUAL(calculate_position(1, 29.97), 33);
  BOOST_CHECK_EQUAL(calculate_position(2, 29.97), 66);
  BOOST_CHECK_EQUAL(calculate_position(3, 29.97), 100);
  BOOST_CHECK_EQUAL(calculate_position(10, 29.97), 333);
  BOOST_CHECK_EQUAL(calculate_position(25, 29.97), 834);
  BOOST_CHECK_EQUAL(calculate_position(517, 29.97), 17250);
  BOOST_CHECK_EQUAL(calculate_position(42619, 29.97), 1422055);
}

BOOST_AUTO_TEST_CASE(should_calculate_position_2)
{
  BOOST_CHECK_EQUAL(calculate_position(0, 25), 0);
  BOOST_CHECK_EQUAL(calculate_position(1, 25), 40);
  BOOST_CHECK_EQUAL(calculate_position(2, 25), 80);
  BOOST_CHECK_EQUAL(calculate_position(3, 25), 120);
  BOOST_CHECK_EQUAL(calculate_position(10, 25), 400);
  BOOST_CHECK_EQUAL(calculate_position(25, 25), 1000);
  BOOST_CHECK_EQUAL(calculate_position(517, 25), 20680);
  BOOST_CHECK_EQUAL(calculate_position(42619, 25), 1704760);
}

BOOST_AUTO_TEST_SUITE_END()
