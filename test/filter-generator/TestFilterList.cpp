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
#include <sstream>

#include "Exceptions.hpp"
#include "FilterList.hpp"
#include "Filters.hpp"

using namespace fg;


#define BOOST_TEST_MODULE filter list
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(insert_should_keep_the_filters_ordered)
{
  FilterList list;

  DelogoFilter* filter1 = new DelogoFilter(10, 15, 100, 20);
  list.insert(0, filter1);
  DrawboxFilter* filter2 = new DrawboxFilter(1, 2, 3, 4);
  list.insert(1000, filter2);
  NullFilter* filter3 = new NullFilter();
  list.insert(400, filter3);

  BOOST_CHECK_EQUAL(list.size(), 3);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 0);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 400);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 1000);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(insert_should_replace_an_existing_filter)
{
  FilterList list;

  NullFilter* filter1 = new NullFilter();
  list.insert(0, filter1);
  DrawboxFilter* filter2 = new DrawboxFilter(1, 2, 3, 4);
  list.insert(500, filter2);

  DelogoFilter* filter3 = new DelogoFilter(10, 15, 100, 20);
  list.insert(500, filter3);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 0);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 500);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(remove_should_remove_an_item)
{
  FilterList list;
  list.insert(400, new DelogoFilter(1, 2, 3, 4));
  list.insert(200, new DrawboxFilter(11, 22, 33, 44));
  list.insert(100, new NullFilter());

  list.remove(200);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 100);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 400);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(remove_should_do_nothing_if_item_does_not_exist)
{
  FilterList list;
  list.insert(100, new NullFilter());
  list.insert(200, new DrawboxFilter(11, 22, 33, 44));

  list.remove(150);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 100);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 200);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(get_by_frame_returning_an_item)
{
  FilterList list;
  list.insert(100, new NullFilter());

  auto maybe = list.get_by_start_frame(100);
  BOOST_CHECK(maybe);
  BOOST_CHECK_EQUAL(maybe->first, 100);
  BOOST_CHECK_EQUAL(maybe->second->type(), FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(get_by_frame_returning_none)
{
  FilterList list;
  list.insert(100, new NullFilter());

  auto maybe = list.get_by_start_frame(200);
  BOOST_CHECK(!maybe);
}


BOOST_AUTO_TEST_CASE(get_by_position_returning_an_item)
{
  FilterList list;
  list.insert(0, new DelogoFilter(1, 2, 3, 4));
  list.insert(200, new DrawboxFilter(11, 22, 33, 44));
  list.insert(100, new NullFilter());
  list.insert(400, new DelogoFilter(99, 88, 77, 66));

  auto maybe0 = list.get_by_position(0);
  BOOST_CHECK(maybe0);
  BOOST_CHECK_EQUAL(maybe0->first, 0);
  BOOST_CHECK_EQUAL(maybe0->second->type(), FilterType::DELOGO);

  auto maybe2 = list.get_by_position(2);
  BOOST_CHECK(maybe2);
  BOOST_CHECK_EQUAL(maybe2->first, 200);
  BOOST_CHECK_EQUAL(maybe2->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(get_by_position_returning_none)
{
  FilterList list;
  list.insert(100, new NullFilter());

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
  list.insert(0, new NullFilter());
  list.insert(400, new NullFilter());
  list.insert(250, new NullFilter());

  BOOST_CHECK_EQUAL(list.get_position(0), 0);
  BOOST_CHECK_EQUAL(list.get_position(250), 1);
  BOOST_CHECK_EQUAL(list.get_position(400), 2);
}


BOOST_AUTO_TEST_CASE(get_position_for_non_existing)
{
  FilterList list;
  list.insert(200, new NullFilter());

  BOOST_CHECK_EQUAL(list.get_position(100), -1);
}


BOOST_AUTO_TEST_CASE(should_load_a_list)
{
  std::istringstream in(
    "0;drawbox;10;20;30;40\n"
    "300;none;\n"
    "600;delogo;100;50;200;80\n");

  FilterList list;
  list.load(in);

  BOOST_CHECK_EQUAL(list.size(), 3);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 0);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 300);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 600);
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
    "0;none;\n"
    "ab;delogo;1;2;3;4\n");

  FilterList list;
  BOOST_CHECK_THROW(list.load(in), fg::InvalidFilterException);
}


BOOST_AUTO_TEST_CASE(should_fail_for_invalid_filter)
{
  std::istringstream in("100;delogo;1;2\n");

  FilterList list;
  BOOST_CHECK_THROW(list.load(in), fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(should_save_the_list)
{
  FilterList list;
  list.insert(0, new DelogoFilter(1, 2, 3, 4));
  list.insert(500, new DrawboxFilter(11, 22, 33, 44));
  list.insert(1500, new NullFilter());
  list.insert(1000, new DrawboxFilter(111, 222, 333, 444));

  std::ostringstream out;
  list.save(out);

  std::string expected =
    "0;delogo;1;2;3;4\n"
    "500;drawbox;11;22;33;44\n"
    "1000;drawbox;111;222;333;444\n"
    "1500;none;\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script)
{
  FilterList list;
  list.insert(0, new DelogoFilter(10, 11, 12, 13));
  list.insert(500, new DrawboxFilter(20, 21, 22, 23));
  list.insert(1000, new NullFilter());
  list.insert(1300, new DrawboxFilter(30, 31, 32, 33));
  list.insert(2000, new DrawboxFilter(40, 41, 42, 43));

  std::ostringstream out;
  list.generate_ffmpeg_script(out);

  std::string expected =
    "delogo=enable='between(n,0,499):x=10:y=11:w=12:h=13,\n"
    "drawbox=enable='between(n,500,999):x=20:y=21:w=22:h=23:c=black:t=fill,\n"
    "drawbox=enable='between(n,1300,1999):x=30:y=31:w=32:h=33:c=black:t=fill,\n"
    "drawbox=enable='gte(n,2000):x=40:y=41:w=42:h=43:c=black:t=fill\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_discard_a_null_filter_at_the_end)
{
  FilterList list;
  list.insert(0, new DelogoFilter(10, 11, 12, 13));
  list.insert(1000, new NullFilter());

  std::ostringstream out;
  list.generate_ffmpeg_script(out);

  std::string expected =
    "delogo=enable='between(n,0,999):x=10:y=11:w=12:h=13,\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}
