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

#include "Filters.hpp"
#include "FilterFactory.hpp"
#include "Exceptions.hpp"


#define BOOST_TEST_MODULE filter factory
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"


BOOST_AUTO_TEST_SUITE(loading_filters)

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
  fg::filter_ptr filter = fg::FilterFactory::load("none;");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(test_load_delogo_filter)
{
  fg::filter_ptr filter = fg::FilterFactory::load("delogo;10;20;30;40");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::DELOGO);
  fg::DelogoFilter* dfilter = dynamic_cast<fg::DelogoFilter*>(filter.get());
  BOOST_CHECK_EQUAL(dfilter->x(), 10);
  BOOST_CHECK_EQUAL(dfilter->y(), 20);
  BOOST_CHECK_EQUAL(dfilter->width(), 30);
  BOOST_CHECK_EQUAL(dfilter->height(), 40);
}


BOOST_AUTO_TEST_CASE(test_load_drawbox_filter)
{
  fg::filter_ptr filter = fg::FilterFactory::load("drawbox;200;100;30;15");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::DRAWBOX);
  fg::DrawboxFilter* dfilter = dynamic_cast<fg::DrawboxFilter*>(filter.get());
  BOOST_CHECK_EQUAL(dfilter->x(), 200);
  BOOST_CHECK_EQUAL(dfilter->y(), 100);
  BOOST_CHECK_EQUAL(dfilter->width(), 30);
  BOOST_CHECK_EQUAL(dfilter->height(), 15);
}


BOOST_AUTO_TEST_CASE(test_load_cut_filter)
{
  fg::filter_ptr filter = fg::FilterFactory::load("cut;");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::CUT);
}


BOOST_AUTO_TEST_CASE(test_load_review_filter)
{
  fg::filter_ptr filter = fg::FilterFactory::load("review;");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::REVIEW);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(creating_filters_from_type)

BOOST_AUTO_TEST_CASE(should_create_a_null_filter_with_no_rectangle)
{
  fg::filter_ptr filter = fg::FilterFactory::create(fg::FilterType::NO_OP);
  BOOST_CHECK(filter->type() == fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(should_create_a_null_filter_discarding_parameters)
{
  fg::filter_ptr filter = fg::FilterFactory::create(fg::FilterType::NO_OP, 1, 2, 3, 4);
  BOOST_CHECK(filter->type() == fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(should_create_a_delogo_filter_with_parameters)
{
  fg::filter_ptr filter = fg::FilterFactory::create(fg::FilterType::DELOGO, 10, 20, 30, 40);
  BOOST_CHECK(filter->type() == fg::FilterType::DELOGO);

  fg::DelogoFilter* delogo = dynamic_cast<fg::DelogoFilter*>(filter.get());
  BOOST_CHECK(delogo->x()      == 10);
  BOOST_CHECK(delogo->y()      == 20);
  BOOST_CHECK(delogo->width()  == 30);
  BOOST_CHECK(delogo->height() == 40);
}


BOOST_AUTO_TEST_CASE(should_fail_if_trying_to_create_a_delogo_filter_without_parameters)
{
  BOOST_CHECK_THROW(fg::FilterFactory::create(fg::FilterType::DELOGO),
                    fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(should_create_a_drawbox_filter_with_parameters)
{
  fg::filter_ptr filter = fg::FilterFactory::create(fg::FilterType::DRAWBOX, 50, 60, 70, 80);
  BOOST_CHECK(filter->type() == fg::FilterType::DRAWBOX);

  fg::DrawboxFilter* drawbox = dynamic_cast<fg::DrawboxFilter*>(filter.get());
  BOOST_CHECK(drawbox->x()      == 50);
  BOOST_CHECK(drawbox->y()      == 60);
  BOOST_CHECK(drawbox->width()  == 70);
  BOOST_CHECK(drawbox->height() == 80);
}


BOOST_AUTO_TEST_CASE(should_fail_if_trying_to_create_a_drawbox_filter_without_parameters)
{
  BOOST_CHECK_THROW(fg::FilterFactory::create(fg::FilterType::DRAWBOX),
                    fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(should_create_a_cut_filter)
{
  fg::filter_ptr filter = fg::FilterFactory::create(fg::FilterType::CUT);
  BOOST_CHECK(filter->type() == fg::FilterType::CUT);
}


BOOST_AUTO_TEST_CASE(should_create_a_review_filter)
{
  fg::filter_ptr filter = fg::FilterFactory::create(fg::FilterType::REVIEW);
  BOOST_CHECK(filter->type() == fg::FilterType::REVIEW);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(converting_filters)

BOOST_AUTO_TEST_CASE(should_convert_from_delogo_to_null_filter)
{
  fg::filter_ptr filter(new fg::DelogoFilter(0, 500, 100, 30));

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::NO_OP);

  BOOST_CHECK(converted->type() == fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(should_convert_from_drawbox_to_null_filter)
{
  fg::filter_ptr filter(new fg::DelogoFilter(0, 0, 10, 10));

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::NO_OP);

  BOOST_CHECK(converted->type() == fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(should_convert_from_null_filter_to_delogo)
{
  fg::filter_ptr filter(new fg::NullFilter());

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::DELOGO);

  BOOST_CHECK(converted->type() == fg::FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(should_convert_from_null_filter_to_drawbox)
{
  fg::filter_ptr filter(new fg::NullFilter());

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::DRAWBOX);

  BOOST_CHECK(converted->type() == fg::FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(should_convert_from_delogo_to_drawbox)
{
  fg::filter_ptr filter(new fg::DelogoFilter(9, 8, 7, 6));

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::DRAWBOX);
  BOOST_CHECK(converted->type() == fg::FilterType::DRAWBOX);

  fg::DrawboxFilter* drawbox = dynamic_cast<fg::DrawboxFilter*>(converted.get());
  BOOST_CHECK_EQUAL(drawbox->x(), 9);
  BOOST_CHECK_EQUAL(drawbox->y(), 8);
  BOOST_CHECK_EQUAL(drawbox->width(), 7);
  BOOST_CHECK_EQUAL(drawbox->height(), 6);
}


BOOST_AUTO_TEST_CASE(should_convert_from_drawbox_to_delogo)
{
  fg::filter_ptr filter(new fg::DrawboxFilter(1, 2, 3, 4));

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::DELOGO);
  BOOST_CHECK(converted->type() == fg::FilterType::DELOGO);

  fg::DelogoFilter* delogo = dynamic_cast<fg::DelogoFilter*>(converted.get());
  BOOST_CHECK_EQUAL(delogo->x(), 1);
  BOOST_CHECK_EQUAL(delogo->y(), 2);
  BOOST_CHECK_EQUAL(delogo->width(), 3);
  BOOST_CHECK_EQUAL(delogo->height(), 4);
}


BOOST_AUTO_TEST_CASE(should_convert_from_delogo_to_cut_filter)
{
  fg::filter_ptr filter(new fg::DelogoFilter(0, 500, 100, 30));

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::CUT);

  BOOST_CHECK(converted->type() == fg::FilterType::CUT);
}


BOOST_AUTO_TEST_CASE(should_convert_from_cut_filter_to_drawbox)
{
  fg::filter_ptr filter(new fg::CutFilter());

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::DRAWBOX);

  BOOST_CHECK(converted->type() == fg::FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(should_convert_from_cut_filter_to_null_filter)
{
  fg::filter_ptr filter(new fg::CutFilter());

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::NO_OP);

  BOOST_CHECK(converted->type() == fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(should_convert_from_review_filter_to_delogo)
{
  fg::filter_ptr filter(new fg::ReviewFilter());

  fg::filter_ptr converted = fg::FilterFactory::convert(filter, fg::FilterType::DELOGO);

  BOOST_CHECK(converted->type() == fg::FilterType::DELOGO);
}

BOOST_AUTO_TEST_SUITE_END()
