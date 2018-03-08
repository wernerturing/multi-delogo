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
#include <memory>
#include <string>
#include <vector>

#include <gtkmm.h>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/Filters.hpp"

#include "EncodeWindow.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE encode window
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"


class GtkInitialization
{
public:
  GtkInitialization()
  {
    Gtk::Main();
  }
};
BOOST_GLOBAL_FIXTURE(GtkInitialization);


namespace mdl {
class EncodeWindowTestFixture
{
public:
  EncodeWindowTestFixture()
    : window(std::move(std::unique_ptr<fg::FilterData>(new fg::FilterData())), 0, 25)
  {
  }

  std::string get_time_remaining(int hours, int minutes, int seconds)
  {
    return window.get_time_remaining({.percentage = 0,
                                      .seconds_elapsed = 0,
                                      .total_seconds_remaining = 0,
                                      .hours_remaining = hours,
                                      .minutes_remaining = minutes,
                                      .seconds_remaining = seconds});
  }

  std::string get_progress_str(double percentage, int hours, int minutes, int seconds)
  {
    return window.get_progress_str({.percentage = percentage,
                                    .seconds_elapsed = 0,
                                    .total_seconds_remaining = 0,
                                    .hours_remaining = hours,
                                    .minutes_remaining = minutes,
                                    .seconds_remaining = seconds});
  }

  EncodeWindow window;
};
}


BOOST_FIXTURE_TEST_SUITE(time_remaining, mdl::EncodeWindowTestFixture)


BOOST_AUTO_TEST_CASE(should_format_time_remaining)
{
  BOOST_TEST(get_time_remaining(0, 0, 1) == "about 0:00:01 left");
  BOOST_TEST(get_time_remaining(0, 0, 59) == "about 0:00:59 left");

  BOOST_TEST(get_time_remaining(0, 3, 23) == "about 0:03:23 left");
  BOOST_TEST(get_time_remaining(0, 59, 59) == "about 0:59:59 left");

  BOOST_TEST(get_time_remaining(2, 5, 0) == "about 2:05:00 left");
}


BOOST_AUTO_TEST_CASE(should_format_progress)
{
  BOOST_TEST(get_progress_str(.13, 0, 7, 21) == "13% done, about 0:07:21 left");
  BOOST_TEST(get_progress_str(.93, 1, 14, 50) == "93% done, about 1:14:50 left");
}


BOOST_AUTO_TEST_SUITE_END()
