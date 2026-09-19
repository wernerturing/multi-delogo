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


BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(501, filter_ptr(new DrawboxFilter(20, 21, 22, 23)));
  list.insert(1001, filter_ptr(new NullFilter()));
  list.insert(1301, filter_ptr(new DrawboxFilter(30, 31, 32, 33)));
  list.insert(2001, filter_ptr(new DrawboxFilter(40, 41, 42, 43)));
  list.insert(2501, filter_ptr(new SpeedFilter(1.5)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 25, boost::none, boost::none, false);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]trim=start_frame=0:end_frame=500,setpts=PTS-STARTPTS,delogo=x=10:y=11:w=12:h=13[vs0];\n"
    "[0:a]atrim=start=0.000:end=20.000,asetpts=PTS-STARTPTS[as0];\n"
    "[0:v]trim=start_frame=500:end_frame=1000,setpts=PTS-STARTPTS,drawbox=x=20:y=21:w=22:h=23:c=black:t=fill[vs1];\n"
    "[0:a]atrim=start=20.000:end=40.000,asetpts=PTS-STARTPTS[as1];\n"
    "[0:v]trim=start_frame=1000:end_frame=1300,setpts=PTS-STARTPTS[vs2];\n"
    "[0:a]atrim=start=40.000:end=52.000,asetpts=PTS-STARTPTS[as2];\n"
    "[0:v]trim=start_frame=1300:end_frame=2000,setpts=PTS-STARTPTS,drawbox=x=30:y=31:w=32:h=33:c=black:t=fill[vs3];\n"
    "[0:a]atrim=start=52.000:end=80.000,asetpts=PTS-STARTPTS[as3];\n"
    "[0:v]trim=start_frame=2000:end_frame=2500,setpts=PTS-STARTPTS,drawbox=x=40:y=41:w=42:h=43:c=black:t=fill[vs4];\n"
    "[0:a]atrim=start=80.000:end=100.000,asetpts=PTS-STARTPTS[as4];\n"
    "[0:v]trim=start_frame=2500,setpts=PTS-STARTPTS,setpts=0.666667*PTS[vs5];\n"
    "[0:a]atrim=start=100.000,asetpts=PTS-STARTPTS,atempo=1.500000[as5];\n"
    "[vs0][as0][vs1][as1][vs2][as2][vs3][as3][vs4][as4][vs5][as5]concat=n=6:v=1:a=1[out_v][out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script_with_scaling)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(501, filter_ptr(new DrawboxFilter(20, 21, 22, 23)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 25, 1280, 720, false);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]trim=start_frame=0:end_frame=500,setpts=PTS-STARTPTS,delogo=x=10:y=11:w=12:h=13,scale=1280:720[vs0];\n"
    "[0:a]atrim=start=0.000:end=20.000,asetpts=PTS-STARTPTS[as0];\n"
    "[0:v]trim=start_frame=500,setpts=PTS-STARTPTS,drawbox=x=20:y=21:w=22:h=23:c=black:t=fill,scale=1280:720[vs1];\n"
    "[0:a]atrim=start=20.000,asetpts=PTS-STARTPTS[as1];\n"
    "[vs0][as0][vs1][as1]concat=n=2:v=1:a=1[out_v][out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script_without_audio)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  list.insert(501, filter_ptr(new DrawboxFilter(20, 21, 22, 23)));
  list.insert(1001, filter_ptr(new NullFilter()));
  list.insert(1301, filter_ptr(new DrawboxFilter(30, 31, 32, 33)));
  list.insert(2001, filter_ptr(new DrawboxFilter(40, 41, 42, 43)));
  list.insert(2501, filter_ptr(new SpeedFilter(1.5)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 25, boost::none, boost::none, true);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]trim=start_frame=0:end_frame=500,setpts=PTS-STARTPTS,delogo=x=10:y=11:w=12:h=13[vs0];\n"
    "[0:v]trim=start_frame=500:end_frame=1000,setpts=PTS-STARTPTS,drawbox=x=20:y=21:w=22:h=23:c=black:t=fill[vs1];\n"
    "[0:v]trim=start_frame=1000:end_frame=1300,setpts=PTS-STARTPTS[vs2];\n"
    "[0:v]trim=start_frame=1300:end_frame=2000,setpts=PTS-STARTPTS,drawbox=x=30:y=31:w=32:h=33:c=black:t=fill[vs3];\n"
    "[0:v]trim=start_frame=2000:end_frame=2500,setpts=PTS-STARTPTS,drawbox=x=40:y=41:w=42:h=43:c=black:t=fill[vs4];\n"
    "[0:v]trim=start_frame=2500,setpts=PTS-STARTPTS,setpts=0.666667*PTS[vs5];\n"
    "[vs0][vs1][vs2][vs3][vs4][vs5]concat=n=6:v=1:a=0[out_v]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(cut_filters_simply_skip_that_segment)
{
  FilterList list;
  list.insert(1, filter_ptr(new CutFilter()));
  list.insert(201, filter_ptr(new NullFilter()));
  list.insert(501, filter_ptr(new CutFilter()));
  list.insert(1001, filter_ptr(new NullFilter()));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1280, 720, 24, boost::none, boost::none, false);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]trim=start_frame=200:end_frame=500,setpts=PTS-STARTPTS[vs0];\n"
    "[0:a]atrim=start=8.333:end=20.833,asetpts=PTS-STARTPTS[as0];\n"
    "[0:v]trim=start_frame=1000,setpts=PTS-STARTPTS[vs1];\n"
    "[0:a]atrim=start=41.667,asetpts=PTS-STARTPTS[as1];\n"
    "[vs0][as0][vs1][as1]concat=n=2:v=1:a=1[out_v][out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_copy_first_segment_unchanged_if_first_filter_does_not_start_at_first_frame)
{
  FilterList list;
  list.insert(101, filter_ptr(new CutFilter()));
  list.insert(201, filter_ptr(new NullFilter()));
  list.insert(501, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1280, 720, 24, boost::none, boost::none, false);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]trim=start_frame=0:end_frame=100,setpts=PTS-STARTPTS[vs0];\n"
    "[0:a]atrim=start=0.000:end=4.167,asetpts=PTS-STARTPTS[as0];\n"
    "[0:v]trim=start_frame=200:end_frame=500,setpts=PTS-STARTPTS[vs1];\n"
    "[0:a]atrim=start=8.333:end=20.833,asetpts=PTS-STARTPTS[as1];\n"
    "[0:v]trim=start_frame=500,setpts=PTS-STARTPTS,delogo=x=10:y=11:w=12:h=13[vs2];\n"
    "[0:a]atrim=start=20.833,asetpts=PTS-STARTPTS[as2];\n"
    "[vs0][as0][vs1][as1][vs2][as2]concat=n=3:v=1:a=1[out_v][out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(should_work_for_a_one_filter_list)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 24, boost::none, boost::none, false);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]trim=start_frame=0,setpts=PTS-STARTPTS,delogo=x=10:y=11:w=12:h=13[vs0];\n"
    "[0:a]atrim=start=0.000,asetpts=PTS-STARTPTS[as0];\n"
    "[vs0][as0]concat=n=1:v=1:a=1[out_v][out_a]";
  BOOST_CHECK_EQUAL(out.str(), expected);
}


BOOST_AUTO_TEST_CASE(fps_should_use_dot_as_decimal_separator_regardless_of_locale)
{
  char* previous_locale = setlocale(LC_NUMERIC, nullptr);
  setlocale(LC_NUMERIC, "pt_BR.UTF-8");

  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(10, 11, 12, 13)));
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1920, 1080, 24, boost::none, boost::none, false);

  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  std::string expected =
    "[0:v]trim=start_frame=0,setpts=PTS-STARTPTS,delogo=x=10:y=11:w=12:h=13[vs0];\n"
    "[0:a]atrim=start=0.000,asetpts=PTS-STARTPTS[as0];\n"
    "[vs0][as0]concat=n=1:v=1:a=1[out_v][out_a]";
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
  std::shared_ptr<ScriptGenerator> g = RegularScriptGenerator::create(list, 1280, 720, 25, boost::none, boost::none, false);

  // Only works after the script has been generated
  std::ostringstream out;
  g->generate_ffmpeg_script(out);

  int result = g->resulting_frames(3000);
  BOOST_TEST(result == 1600);
}
