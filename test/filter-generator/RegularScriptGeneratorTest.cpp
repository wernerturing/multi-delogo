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
#include <clocale>
#include <memory>
#include <string>
#include <sstream>

#include "FilterList.hpp"
#include "Filters.hpp"
#include "RegularScriptGenerator.hpp"

using namespace fg;


#define BOOST_TEST_MODULE regular ffmpeg script generator
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(affects_audio_without_audio_affecting_filters)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(501, filter_ptr(new DrawboxFilter(20, 21, 22, 23)));
  list.insert(1001, filter_ptr(new NullFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 1);

  BOOST_TEST(!g->affects_audio());
}


BOOST_AUTO_TEST_CASE(affects_audio_with_audio_affecting_filters)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(501, filter_ptr(new CutFilter()));
  list.insert(1001, filter_ptr(new NullFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 1);

  BOOST_TEST(g->affects_audio());
}


BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(501, filter_ptr(new DrawboxFilter(20, 21, 22, 23)));
  list.insert(1001, filter_ptr(new NullFilter()));
  list.insert(1301, filter_ptr(new DrawboxFilter(30, 31, 32, 33)));
  list.insert(2001, filter_ptr(new DrawboxFilter(40, 41, 42, 43)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 1);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]\n"
    "delogo=enable='between(n,0,499)':x=10:y=11:w=12:h=13,\n"
    "drawbox=enable='between(n,500,999)':x=20:y=21:w=22:h=23:c=black:t=fill,\n"
    "drawbox=enable='between(n,1300,1999)':x=30:y=31:w=32:h=33:c=black:t=fill,\n"
    "drawbox=enable='gte(n,2000)':x=40:y=41:w=42:h=43:c=black:t=fill\n"
    "[out_v]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_discard_a_null_filter_at_the_end)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(1001, filter_ptr(new NullFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 1);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]\n"
    "delogo=enable='between(n,0,999)':x=10:y=11:w=12:h=13\n"
    "[out_v]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_work_for_a_one_filter_list)
{
  FilterList list;
  list.insert(50, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 1);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]\n"
    "delogo=enable='gte(n,49)':x=10:y=11:w=12:h=13\n"
    "[out_v]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_generate_script_with_cut_filter)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(601, filter_ptr(new CutFilter()));
  list.insert(1001, filter_ptr(new DrawboxFilter(20, 21, 22, 23)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1280, 720, 25);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]\n"
    "delogo=enable='between(n,0,599)':x=10:y=11:w=12:h=13,\n"
    "drawbox=enable='gte(n,1000)':x=20:y=21:w=22:h=23:c=black:t=fill,\n"
    "select='not(between(n,600,999))',setpts=N/FRAME_RATE/TB\n"
    "[out_v];\n"
    "[0:a]aselect='not(between(t,600/25.000000,999/25.000000))',asetpts=N/SR/TB[out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_generate_script_with_only_cut_filters)
{
  FilterList list;
  list.insert(101, filter_ptr(new CutFilter()));
  list.insert(201, filter_ptr(new NullFilter()));
  list.insert(501, filter_ptr(new CutFilter()));
  list.insert(1001, filter_ptr(new NullFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1280, 720, 24);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]\n"
    "select='not(between(n,100,199)+between(n,500,999))',setpts=N/FRAME_RATE/TB\n"
    "[out_v];\n"
    "[0:a]aselect='not(between(t,100/24.000000,199/24.000000)+between(t,500/24.000000,999/24.000000))',asetpts=N/SR/TB[out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_generate_script_with_cut_filter_at_the_end)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(601, filter_ptr(new CutFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 640, 480, 29.97);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]\n"
    "delogo=enable='between(n,0,599)':x=10:y=11:w=12:h=13,\n"
    "select='not(gte(n,600))',setpts=N/FRAME_RATE/TB\n"
    "[out_v];\n"
    "[0:a]aselect='not(gte(t,600/29.970000))',asetpts=N/SR/TB[out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(fps_should_use_dot_as_decimal_separator_regardless_of_locale)
{
  char* previous_locale = setlocale(LC_NUMERIC, nullptr);
  setlocale(LC_NUMERIC, "pt_BR.UTF-8");

  FilterList list;
  list.insert(501, filter_ptr(new CutFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 29.97);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]\n"
    "select='not(gte(n,500))',setpts=N/FRAME_RATE/TB\n"
    "[out_v];\n"
    "[0:a]aselect='not(gte(t,500/29.970000))',asetpts=N/SR/TB[out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);

  setlocale(LC_NUMERIC, previous_locale);
}


BOOST_AUTO_TEST_CASE(should_calculate_number_of_frames_in_result)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(601, filter_ptr(new CutFilter()));
  list.insert(1001, filter_ptr(new DrawboxFilter(20, 21, 22, 23)));
  list.insert(2001, filter_ptr(new CutFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1280, 720, 25);

  // Only works after the script has been generated
  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  int result = g->resulting_frames(3000);
  BOOST_TEST(result == 1600);
}
