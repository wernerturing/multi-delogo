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


#define BOOST_TEST_MODULE cut filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_type)
{
  fg::CutFilter filter;

  BOOST_CHECK_EQUAL(filter.type(), fg::FilterType::CUT);
}


BOOST_AUTO_TEST_CASE(test_name)
{
  fg::CutFilter filter;

  BOOST_CHECK_EQUAL(filter.name(), "cut");
}


BOOST_AUTO_TEST_CASE(cut_filter_affects_audio)
{
  fg::CutFilter filter;
  BOOST_TEST(filter.affects_audio());
}


BOOST_AUTO_TEST_CASE(test_save_str)
{
  fg::CutFilter filter;

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "cut;");
}


BOOST_AUTO_TEST_CASE(test_load)
{
  std::shared_ptr<fg::CutFilter> filter = fg::CutFilter::load("");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::CUT);
}


BOOST_AUTO_TEST_CASE(test_load_with_invalid_parameters)
{
  BOOST_CHECK_THROW(fg::CutFilter::load("1;2"), fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(test_ffmpeg_str)
{
  fg::CutFilter filter;

  std::string ffmpeg(filter.ffmpeg_str("<BETWEEN>", 1920, 1080));

  BOOST_CHECK_EQUAL(ffmpeg, "");
}

