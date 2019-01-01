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
#include <vector>

#include <gtkmm.h>

#include "filter-generator/FilterList.hpp"
#include "filter-generator/Filters.hpp"
#include "filter-generator/RegularScriptGenerator.hpp"

#include "FFmpegExecutor.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE FFmpeg executor
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"


namespace mdl {
class FFmpegExecutorTestFixture
{
public:
  FFmpegExecutorTestFixture()
  {
    ffmpeg.set_generator(fg::RegularScriptGenerator::create(filters, 1920, 1080, 25));
    ffmpeg.set_input_file("input.mp4");
    ffmpeg.set_output_file("output.mkv");
  }

  void add_filter(int start_frame, fg::filter_ptr filter)
  {
    filters.insert(start_frame, filter);
  }

  std::vector<std::string> get_ffmpeg_cmd_line()
  {
    return ffmpeg.get_ffmpeg_cmd_line("filters.ffm");
  }

  void set_output_frames(int frames)
  {
    ffmpeg.total_frames_output_ = frames;
  }

  double get_progress_percentage(const std::string& ffmpeg_stats)
  {
    return ffmpeg.get_progress(ffmpeg_stats).percentage;
  }

  fg::FilterList filters;
  FFmpegExecutor ffmpeg;
};
}

BOOST_FIXTURE_TEST_SUITE(ffmpeg_command_line, mdl::FFmpegExecutorTestFixture)

BOOST_AUTO_TEST_CASE(test_ffmpeg_command_line_h264_copy_audio)
{
  ffmpeg.set_codec(FFmpegExecutor::Codec::H264);
  ffmpeg.set_quality(20);

  std::vector<std::string> expected{
    "ffmpeg",
    "-y", "-v", "error", "-stats",
    "-i", "input.mp4",
    "-filter_complex_script", "filters.ffm",
    "-map", "[out_v]", "-c:v", "libx264", "-crf", "20",
    "-map", "0:a?", "-c:a", "copy",
    "output.mkv"};
  BOOST_TEST(get_ffmpeg_cmd_line() == expected,
             boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(test_ffmpeg_command_line_h265_copy_audio)
{
  ffmpeg.set_codec(FFmpegExecutor::Codec::H265);
  ffmpeg.set_quality(25);

  std::vector<std::string> expected{
    "ffmpeg",
    "-y", "-v", "error", "-stats",
    "-i", "input.mp4",
    "-filter_complex_script", "filters.ffm",
    "-map", "[out_v]", "-c:v", "libx265", "-crf", "25",
    "-map", "0:a?", "-c:a", "copy",
    "output.mkv"};
  BOOST_TEST(get_ffmpeg_cmd_line() == expected,
             boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(test_ffmpeg_command_line_h264_reencode_audio)
{
  add_filter(1000, fg::filter_ptr(new fg::CutFilter()));
  ffmpeg.set_codec(FFmpegExecutor::Codec::H264);
  ffmpeg.set_quality(20);

  std::vector<std::string> expected{
    "ffmpeg",
    "-y", "-v", "error", "-stats",
    "-i", "input.mp4",
    "-filter_complex_script", "filters.ffm",
    "-map", "[out_v]", "-c:v", "libx264", "-crf", "20",
    "-map", "[out_a]", "-c:a", "aac", "-b:a", "192k",
    "output.mkv"};
  BOOST_TEST(get_ffmpeg_cmd_line() == expected,
             boost::test_tools::per_element());
}


BOOST_AUTO_TEST_CASE(test_ffmpeg_command_line_mp4_output)
{
  ffmpeg.set_codec(FFmpegExecutor::Codec::H264);
  ffmpeg.set_quality(20);
  ffmpeg.set_output_file("output.mp4");

  std::vector<std::string> expected{
    "ffmpeg",
    "-y", "-v", "error", "-stats",
    "-i", "input.mp4",
    "-filter_complex_script", "filters.ffm",
    "-map", "[out_v]", "-c:v", "libx264", "-crf", "20",
    "-map", "0:a?", "-c:a", "copy",
    "-movflags", "+faststart",
    "output.mp4"};
  BOOST_TEST(get_ffmpeg_cmd_line() == expected,
             boost::test_tools::per_element());
}

BOOST_AUTO_TEST_SUITE_END()


BOOST_FIXTURE_TEST_SUITE(progress_percentage, mdl::FFmpegExecutorTestFixture,
                         * boost::unit_test::tolerance(0.001))

BOOST_AUTO_TEST_CASE(should_calculate_progress)
{
  set_output_frames(15372);

  double p = get_progress_percentage("frame=  4238 fps= 36 q=31.0 size=    2048kB time=00:00:19.06 bitrate= 880.1kbits/s speed=0.605x");
  BOOST_TEST(p == 0.27569);
}


BOOST_AUTO_TEST_CASE(should_return_negative_for_invalid_line)
{
  set_output_frames(15372);

  double p = get_progress_percentage("Some random string");
  BOOST_TEST(p < 0);
}

BOOST_AUTO_TEST_SUITE_END()
