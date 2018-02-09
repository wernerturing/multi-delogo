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

#include "Filters.hpp"
#include "FilterFactory.hpp"
#include "Exceptions.hpp"


#define BOOST_TEST_MODULE filter factory
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_load_with_invalid_string)
{
  BOOST_CHECK_THROW(fg::FilterFactory::load("none"), fg::InvalidFilterException);
}

BOOST_AUTO_TEST_CASE(test_load_with_unknown_filter)
{
  BOOST_CHECK_THROW(fg::FilterFactory::load("blur;1;2;3;4"), fg::UnknownFilterException);
}

BOOST_AUTO_TEST_CASE(test_load_null_filter)
{
  fg::Filter* filter = fg::FilterFactory::load("none;");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::NO_OP);
}

BOOST_AUTO_TEST_CASE(test_load_delogo_filter)
{
  fg::Filter* filter = fg::FilterFactory::load("delogo;10;20;30;40");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::DELOGO);
  fg::DelogoFilter* dfilter = dynamic_cast<fg::DelogoFilter*>(filter);
  BOOST_CHECK_EQUAL(dfilter->x(), 10);
  BOOST_CHECK_EQUAL(dfilter->y(), 20);
  BOOST_CHECK_EQUAL(dfilter->width(), 30);
  BOOST_CHECK_EQUAL(dfilter->height(), 40);
}

BOOST_AUTO_TEST_CASE(test_load_drawbox_filter)
{
  fg::Filter* filter = fg::FilterFactory::load("drawbox;200;100;30;15");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::DRAWBOX);
  fg::DrawboxFilter* dfilter = dynamic_cast<fg::DrawboxFilter*>(filter);
  BOOST_CHECK_EQUAL(dfilter->x(), 200);
  BOOST_CHECK_EQUAL(dfilter->y(), 100);
  BOOST_CHECK_EQUAL(dfilter->width(), 30);
  BOOST_CHECK_EQUAL(dfilter->height(), 15);
}
