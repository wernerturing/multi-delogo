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
#include <sstream>

#include "Exceptions.hpp"
#include "FilterList.hpp"
#include "Filters.hpp"

using namespace fg;


#define BOOST_TEST_MODULE filter list
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_empty)
{
  FilterList list;

  BOOST_TEST(list.empty());

  list.insert(1, filter_ptr(new NullFilter()));
  BOOST_TEST(!list.empty());
}


BOOST_AUTO_TEST_CASE(insert_should_keep_the_filters_ordered)
{
  FilterList list;

  DelogoFilter* filter1 = new DelogoFilter(10, 15, 100, 20);
  list.insert(1, filter_ptr(filter1));
  DrawboxFilter* filter2 = new DrawboxFilter(1, 2, 3, 4);
  list.insert(1001, filter_ptr(filter2));
  NullFilter* filter3 = new NullFilter();
  list.insert(401, filter_ptr(filter3));

  BOOST_CHECK_EQUAL(list.size(), 3);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 1);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 401);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 1001);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(insert_should_replace_an_existing_filter)
{
  FilterList list;

  NullFilter* filter1 = new NullFilter();
  list.insert(1, filter_ptr(filter1));
  DrawboxFilter* filter2 = new DrawboxFilter(1, 2, 3, 4);
  list.insert(501, filter_ptr(filter2));

  DelogoFilter* filter3 = new DelogoFilter(10, 15, 100, 20);
  list.insert(501, filter_ptr(filter3));

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 1);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 501);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(remove_should_remove_an_item)
{
  FilterList list;
  list.insert(401, filter_ptr(new DelogoFilter(1, 2, 3, 4)));
  list.insert(201, filter_ptr(new DrawboxFilter(11, 22, 33, 44)));
  list.insert(101, filter_ptr(new NullFilter()));

  list.remove(201);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 101);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 401);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(remove_should_do_nothing_if_item_does_not_exist)
{
  FilterList list;
  list.insert(101, filter_ptr(new NullFilter()));
  list.insert(201, filter_ptr(new DrawboxFilter(11, 22, 33, 44)));

  list.remove(151);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 101);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 201);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(should_change_start_frame)
{
  FilterList list;
  list.insert(101, filter_ptr(new DrawboxFilter(11, 22, 33, 44)));
  list.insert(201, filter_ptr(new NullFilter()));
  list.insert(301, filter_ptr(new DelogoFilter(99, 88, 77, 66)));

  list.change_start_frame(201, 401);

  BOOST_CHECK_EQUAL(list.size(), 3);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 101);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 301);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 401);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(change_start_frame_should_overwrite_an_existing_filter)
{
  FilterList list;
  list.insert(101, filter_ptr(new DrawboxFilter(11, 22, 33, 44)));
  list.insert(201, filter_ptr(new NullFilter()));
  list.insert(301, filter_ptr(new DelogoFilter(99, 88, 77, 66)));

  list.change_start_frame(201, 301);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 101);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 301);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(change_start_frame_should_do_nothing_if_item_does_not_exist)
{
  FilterList list;
  list.insert(101, filter_ptr(new NullFilter()));
  list.insert(201, filter_ptr(new DrawboxFilter(11, 22, 33, 44)));

  list.change_start_frame(151, 201);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 101);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 201);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(get_by_frame_returning_an_item)
{
  FilterList list;
  list.insert(101, filter_ptr(new NullFilter()));

  auto maybe = list.get_by_start_frame(101);
  BOOST_REQUIRE(maybe);
  BOOST_CHECK_EQUAL(maybe->first, 101);
  BOOST_CHECK_EQUAL(maybe->second->type(), FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(get_by_frame_returning_none)
{
  FilterList list;
  list.insert(101, filter_ptr(new NullFilter()));

  auto maybe = list.get_by_start_frame(201);
  BOOST_CHECK(!maybe);
}


BOOST_AUTO_TEST_CASE(get_by_position_returning_an_item)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(1, 2, 3, 4)));
  list.insert(201, filter_ptr(new DrawboxFilter(11, 22, 33, 44)));
  list.insert(101, filter_ptr(new NullFilter()));
  list.insert(401, filter_ptr(new DelogoFilter(99, 88, 77, 66)));

  auto maybe0 = list.get_by_position(0);
  BOOST_REQUIRE(maybe0);
  BOOST_CHECK_EQUAL(maybe0->first, 1);
  BOOST_CHECK_EQUAL(maybe0->second->type(), FilterType::DELOGO);

  auto maybe2 = list.get_by_position(2);
  BOOST_REQUIRE(maybe2);
  BOOST_CHECK_EQUAL(maybe2->first, 201);
  BOOST_CHECK_EQUAL(maybe2->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(get_by_position_returning_none)
{
  FilterList list;
  list.insert(101, filter_ptr(new NullFilter()));

  auto maybe1 = list.get_by_position(1);
  BOOST_CHECK(!maybe1);

  auto maybe2 = list.get_by_position(2);
  BOOST_CHECK(!maybe2);

  auto maybe20 = list.get_by_position(20);
  BOOST_CHECK(!maybe20);
}


BOOST_AUTO_TEST_CASE(get_by_position_on_empty_list)
{
  FilterList list;

  auto maybe0 = list.get_by_position(0);
  BOOST_CHECK(!maybe0);

  auto maybe2 = list.get_by_position(2);
  BOOST_CHECK(!maybe2);
}


BOOST_AUTO_TEST_CASE(get_position_for_existing_item)
{
  FilterList list;
  list.insert(1, filter_ptr(new NullFilter()));
  list.insert(401, filter_ptr(new NullFilter()));
  list.insert(251, filter_ptr(new NullFilter()));

  BOOST_CHECK_EQUAL(list.get_position(1), 0);
  BOOST_CHECK_EQUAL(list.get_position(251), 1);
  BOOST_CHECK_EQUAL(list.get_position(401), 2);
}


BOOST_AUTO_TEST_CASE(get_position_for_non_existing)
{
  FilterList list;
  list.insert(201, filter_ptr(new NullFilter()));

  BOOST_CHECK_EQUAL(list.get_position(100), -1);
}


BOOST_AUTO_TEST_CASE(get_filter_for_frame_returns_filter_applied_to_that_frame)
{
  FilterList list;
  list.insert(101, filter_ptr(new NullFilter()));
  list.insert(401, filter_ptr(new NullFilter()));
  list.insert(251, filter_ptr(new NullFilter()));
  list.insert(651, filter_ptr(new NullFilter()));

  auto maybe30 = list.get_filter_for_frame(30);
  BOOST_CHECK(!maybe30);

  auto maybe251 = list.get_filter_for_frame(251);
  BOOST_REQUIRE(maybe251);
  BOOST_CHECK_EQUAL(maybe251->first, 251);

  auto maybe252 = list.get_filter_for_frame(252);
  BOOST_REQUIRE(maybe252);
  BOOST_CHECK_EQUAL(maybe252->first, 251);

  auto maybe430 = list.get_filter_for_frame(430);
  BOOST_REQUIRE(maybe430);
  BOOST_CHECK_EQUAL(maybe430->first, 401);

  auto maybe1000 = list.get_filter_for_frame(1000);
  BOOST_REQUIRE(maybe1000);
  BOOST_CHECK_EQUAL(maybe1000->first, 651);
}


BOOST_AUTO_TEST_CASE(get_filter_for_frame_on_empty_list)
{
  FilterList list;

  auto maybe250 = list.get_filter_for_frame(250);
  BOOST_CHECK(!maybe250);
}


BOOST_AUTO_TEST_CASE(get_filter_for_frame_on_list_with_one_element)
{
  FilterList list;
  list.insert(101, filter_ptr(new NullFilter()));

  auto maybe50 = list.get_filter_for_frame(50);
  BOOST_CHECK(!maybe50);

  auto maybe150 = list.get_filter_for_frame(150);
  BOOST_REQUIRE(maybe150);
  BOOST_CHECK_EQUAL(maybe150->first, 101);
}


BOOST_AUTO_TEST_CASE(should_return_true_for_has_review_when_there_is_at_least_one_review_filter)
{
  FilterList list;
  list.insert(51, filter_ptr(new DelogoFilter(1, 2, 3, 4)));
  list.insert(101, filter_ptr(new ReviewFilter()));
  list.insert(201, filter_ptr(new DelogoFilter(10, 20, 30, 40)));

  BOOST_TEST(list.has_review_filter());
}


BOOST_AUTO_TEST_CASE(should_return_false_for_has_review_when_there_is_no_review_filter)
{
  FilterList list;
  list.insert(51, filter_ptr(new DelogoFilter(1, 2, 3, 4)));
  list.insert(201, filter_ptr(new DelogoFilter(10, 20, 30, 40)));

  BOOST_TEST(!list.has_review_filter());
}


BOOST_AUTO_TEST_CASE(should_load_a_list)
{
  std::istringstream in(
    "1;drawbox;10;20;30;40\n"
    "301;none;\n"
    "601;delogo;100;50;200;80\n");

  FilterList list;
  list.load(in);

  BOOST_CHECK_EQUAL(list.size(), 3);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 1);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 301);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 601);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(should_load_a_list_with_windows_line_ending)
{
  std::istringstream in(
    "1;drawbox;10;20;30;40\r\n"
    "301;none;\r\n"
    "601;delogo;100;50;200;80\r\n");

  FilterList list;
  list.load(in);

  BOOST_CHECK_EQUAL(list.size(), 3);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 1);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 301);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 601);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(should_fail_for_line_without_frame_and_filter)
{
  std::istringstream in("none\n");

  FilterList list;
  BOOST_CHECK_THROW(list.load(in), fg::InvalidFilterException);
}


BOOST_AUTO_TEST_CASE(should_fail_if_frame_is_not_numeric)
{
  std::istringstream in(
    "1;none;\r\n"
    "ab;delogo;1;2;3;4\r\n");

  FilterList list;
  BOOST_CHECK_THROW(list.load(in), fg::InvalidFilterException);
}


BOOST_AUTO_TEST_CASE(should_fail_for_invalid_filter)
{
  std::istringstream in("101;delogo;1;2\n");

  FilterList list;
  BOOST_CHECK_THROW(list.load(in), fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(should_save_the_list)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(1, 2, 3, 4)));
  list.insert(501, filter_ptr(new DrawboxFilter(11, 22, 33, 44)));
  list.insert(1501, filter_ptr(new NullFilter()));
  list.insert(1001, filter_ptr(new DrawboxFilter(111, 222, 333, 444)));

  std::ostringstream out;
  list.save(out);

  std::string expected =
    "1;delogo;1;2;3;4\n"
    "501;drawbox;11;22;33;44\n"
    "1001;drawbox;111;222;333;444\n"
    "1501;none;\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}
