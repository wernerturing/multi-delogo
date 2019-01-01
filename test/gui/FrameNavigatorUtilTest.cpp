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
