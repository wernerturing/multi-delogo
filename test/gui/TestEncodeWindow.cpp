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
    : window(std::move(std::unique_ptr<fg::FilterData>(new fg::FilterData())), 0)
  {
    window.filter_data_->set_movie_file("input.mp4");
    window.txt_file_.set_text("output.mp4");
  }

  void set_codec_h264()
  {
    window.codec_ = EncodeWindow::Codec::H264;
  }

  void set_codec_h265()
  {
    window.codec_ = EncodeWindow::Codec::H265;
  }

  void set_quality(int quality)
  {
    window.txt_quality_.set_value(quality);
  }

  std::vector<std::string> get_ffmpeg_cmd_line()
  {
    return window.get_ffmpeg_cmd_line("filters.ffm");
  }

  void set_total_frames(int frames)
  {
    window.total_frames_ = frames;
  }

  double get_progress_percentage(const std::string& ffmpeg_stats)
  {
    return window.get_progress(ffmpeg_stats).percentage;
  }

  std::string get_time_remaining(int seconds_remaining)
  {
    return window.get_time_remaining(seconds_remaining);
  }

  int calculate_seconds_remaining(double percentage, int seconds_elapsed)
  {
    return window.calculate_seconds_remaining({.percentage = percentage, .seconds_elapsed = seconds_elapsed});
  }

  std::string get_progress_str(double percentage, int seconds_elapsed)
  {
    return window.get_progress_str({.percentage = percentage, .seconds_elapsed = seconds_elapsed});
  }

  EncodeWindow window;
};
}

BOOST_FIXTURE_TEST_SUITE(ffmpeg_command_line, mdl::EncodeWindowTestFixture)

BOOST_AUTO_TEST_CASE(test_ffmpeg_command_line_h264)
{
  set_codec_h264();
  set_quality(20);

  std::vector<std::string> expected{
    "ffmpeg",
    "-y", "-v", "quiet", "-stats",
    "-i", "input.mp4",
    "-filter_complex_script", "filters.ffm",
    "-map", "[out_v]", "-c:v", "libx264", "-crf", "20",
    "-map", "0:a?", "-c:a", "copy",
    "output.mp4"};
  BOOST_TEST(get_ffmpeg_cmd_line() == expected,
             boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(test_ffmpeg_command_line_h265)
{
  set_codec_h265();
  set_quality(25);

  std::vector<std::string> expected{
    "ffmpeg",
    "-y", "-v", "quiet", "-stats",
    "-i", "input.mp4",
    "-filter_complex_script", "filters.ffm",
    "-map", "[out_v]", "-c:v", "libx265", "-crf", "25",
    "-map", "0:a?", "-c:a", "copy",
    "output.mp4"};
  BOOST_TEST(get_ffmpeg_cmd_line() == expected,
             boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(progress_percentage, mdl::EncodeWindowTestFixture,
                         * boost::unit_test::tolerance(0.001))

BOOST_AUTO_TEST_CASE(should_calculate_progress)
{
  set_total_frames(15372);

  double p = get_progress_percentage("frame=  4238 fps= 36 q=31.0 size=    2048kB time=00:00:19.06 bitrate= 880.1kbits/s speed=0.605x");
  BOOST_TEST(p == 0.27569);
}


BOOST_AUTO_TEST_CASE(should_return_negative_for_invalid_line)
{
  set_total_frames(15372);

  double p = get_progress_percentage("Some random string");
  BOOST_TEST(p < 0);
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(time_remaining, mdl::EncodeWindowTestFixture)

BOOST_AUTO_TEST_CASE(should_calculate_time_remaining)
{
  BOOST_TEST(calculate_seconds_remaining(.13, 66) == 441);
  BOOST_TEST(calculate_seconds_remaining(.93, 11832) == 890);
}


BOOST_AUTO_TEST_CASE(should_format_time_remaining)
{
  BOOST_TEST(get_time_remaining(1) == "about 0:00:01 left");
  BOOST_TEST(get_time_remaining(59) == "about 0:00:59 left");

  BOOST_TEST(get_time_remaining(3*60 + 23) == "about 0:03:23 left");
  BOOST_TEST(get_time_remaining(59*60 + 59) == "about 0:59:59 left");

  BOOST_TEST(get_time_remaining(2*60*60 + 5*60) == "about 2:05:00 left");
}


BOOST_AUTO_TEST_CASE(should_format_progress)
{
  BOOST_TEST(get_progress_str(.13, 66) == "13% done, about 0:07:21 left");
  BOOST_TEST(get_progress_str(.93, 11832) == "93% done, about 0:14:50 left");
}


BOOST_AUTO_TEST_SUITE_END()
