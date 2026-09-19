/*
 * Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
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


#define BOOST_TEST_MODULE speed filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_name)
{
  fg::SpeedFilter filter(2);

  BOOST_CHECK_EQUAL(filter.name(), "speed");
}


BOOST_AUTO_TEST_CASE(test_construction)
{
  fg::SpeedFilter filter(2);

  BOOST_CHECK_EQUAL(filter.type(), fg::FilterType::SPEED);
  BOOST_CHECK_EQUAL(filter.factor(), 2);
}


BOOST_AUTO_TEST_CASE(test_load)
{
  std::shared_ptr<fg::SpeedFilter> filter = fg::SpeedFilter::load("1.5");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::SPEED);
  BOOST_CHECK_EQUAL(filter->factor(), 1.5);
}


BOOST_AUTO_TEST_CASE(test_load_with_too_little_params)
{
  BOOST_CHECK_THROW(fg::SpeedFilter::load(""),
                    fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(test_load_with_too_many_params)
{
  BOOST_CHECK_THROW(fg::SpeedFilter::load("1;2"),
                    fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(test_load_with_invalid_parameter)
{
  BOOST_CHECK_THROW(fg::SpeedFilter::load("abc"),
                    fg::InvalidParametersException);
}


BOOST_AUTO_TEST_CASE(test_save_str)
{
  fg::SpeedFilter filter(1.5);

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "speed;1.500000");
}


BOOST_AUTO_TEST_CASE(save_str_should_use_dot_regardless_of_locale)
{
  char* previous_locale = setlocale(LC_NUMERIC, nullptr);
  setlocale(LC_NUMERIC, "pt_BR.UTF-8");

  fg::SpeedFilter filter(1.33);

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "speed;1.330000");

  setlocale(LC_NUMERIC, previous_locale);
}


BOOST_AUTO_TEST_CASE(test_ffmpeg_str)
{
  fg::SpeedFilter filter(2);

  std::string ffmpeg(filter.ffmpeg_str(1280, 720));

  BOOST_CHECK_EQUAL(ffmpeg, "setpts=0.500000*PTS");
}


BOOST_AUTO_TEST_CASE(ffmpeg_str_should_use_dot_regardless_of_locale)
{
  char* previous_locale = setlocale(LC_NUMERIC, nullptr);
  setlocale(LC_NUMERIC, "pt_BR.UTF-8");

  fg::SpeedFilter filter(1.5);

  std::string ffmpeg(filter.ffmpeg_str(1280, 720));

  BOOST_CHECK_EQUAL(ffmpeg, "setpts=0.666667*PTS");

  setlocale(LC_NUMERIC, previous_locale);
}


BOOST_AUTO_TEST_CASE(test_ffmpeg_audio_str)
{
  fg::SpeedFilter filter(1.5);

  std::string ffmpeg(filter.ffmpeg_audio_str());

  BOOST_CHECK_EQUAL(ffmpeg, "atempo=1.500000");
}


BOOST_AUTO_TEST_CASE(ffmpeg_audio_str_should_use_dot_regardless_of_locale)
{
  char* previous_locale = setlocale(LC_NUMERIC, nullptr);
  setlocale(LC_NUMERIC, "pt_BR.UTF-8");

  fg::SpeedFilter filter(1.75);

  std::string ffmpeg(filter.ffmpeg_audio_str());

  BOOST_CHECK_EQUAL(ffmpeg, "atempo=1.750000");

  setlocale(LC_NUMERIC, previous_locale);
}
