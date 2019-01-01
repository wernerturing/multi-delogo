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

#include "ETRProgressBar.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE estimated time remaining progress bar
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


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
class ETRProgressBarTestFixture
{
public:
  std::string get_time_remaining(int hours, int minutes, int seconds)
  {
    return progress.get_time_remaining({.percentage = 0,
                                        .seconds_elapsed = 0,
                                        .total_seconds_remaining = get_total(hours, minutes, seconds),
                                        .hours_remaining = hours,
                                        .minutes_remaining = minutes,
                                        .seconds_remaining = seconds});
  }


  std::string get_progress_str(double percentage, int hours, int minutes, int seconds)
  {
    return progress.get_progress_str({.percentage = percentage,
                                      .seconds_elapsed = 0,
                                      .total_seconds_remaining = get_total(hours, minutes, seconds),
                                      .hours_remaining = hours,
                                      .minutes_remaining = minutes,
                                      .seconds_remaining = seconds});
  }

  ETRProgressBar progress;

private:
  int get_total(int hours, int minutes, int seconds)
  {
    return hours*60*60 + minutes*60 + seconds;
  }
};
}


BOOST_AUTO_TEST_SUITE(time_remaining)

BOOST_AUTO_TEST_CASE(should_calculate_time_remaining_1)
{
  Progress p;
  p.percentage = .13;
  p.seconds_elapsed = 66;

  p.calculate_time_remaining();

  BOOST_TEST(p.total_seconds_remaining == 441);
  BOOST_TEST(p.hours_remaining == 0);
  BOOST_TEST(p.minutes_remaining == 7);
  BOOST_TEST(p.seconds_remaining == 21);
}


BOOST_AUTO_TEST_CASE(should_calculate_time_remaining_2)
{
  Progress p;
  p.percentage = .53;
  p.seconds_elapsed = 11832;

  p.calculate_time_remaining();

  BOOST_TEST(p.total_seconds_remaining == 10492);
  BOOST_TEST(p.hours_remaining == 2);
  BOOST_TEST(p.minutes_remaining == 54);
  BOOST_TEST(p.seconds_remaining == 52);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(progress_string, mdl::ETRProgressBarTestFixture)

BOOST_AUTO_TEST_CASE(should_format_time_remaining)
{
  // Less than one minute: seconds
  BOOST_TEST(get_time_remaining(0, 0, 1) == "about 1 second left");
  BOOST_TEST(get_time_remaining(0, 0, 59) == "about 59 seconds left");

  // Less than 30 minutes: minutes
  BOOST_TEST(get_time_remaining(0, 1, 00) == "about 1 minute left");
  BOOST_TEST(get_time_remaining(0, 1, 29) == "about 1 minute left");
  BOOST_TEST(get_time_remaining(0, 1, 30) == "about 2 minutes left");
  BOOST_TEST(get_time_remaining(0, 12, 43) == "about 13 minutes left");
  BOOST_TEST(get_time_remaining(0, 29, 59) == "about 30 minutes left");

  // Over 30 minutes: hours
  BOOST_TEST(get_time_remaining(0, 30, 00) == "about 1 hour left");
  BOOST_TEST(get_time_remaining(1, 29, 59) == "about 1 hour left");
  BOOST_TEST(get_time_remaining(1, 30, 00) == "about 2 hours left");
  BOOST_TEST(get_time_remaining(5, 12, 19) == "about 5 hours left");
  BOOST_TEST(get_time_remaining(8, 48, 23) == "about 9 hours left");
}


BOOST_AUTO_TEST_CASE(should_format_progress)
{
  BOOST_TEST(get_progress_str(.13, 0, 7, 21) == "13% done, about 7 minutes left");
  BOOST_TEST(get_progress_str(.93, 1, 14, 50) == "93% done, about 1 hour left");
}


BOOST_AUTO_TEST_SUITE_END()
