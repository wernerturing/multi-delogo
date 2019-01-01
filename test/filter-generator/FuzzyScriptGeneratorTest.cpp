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
#include <sstream>
#include <regex>
#include <limits>

#include "FilterList.hpp"
#include "Filters.hpp"
#include "FuzzyScriptGenerator.hpp"

using namespace fg;


#define BOOST_TEST_MODULE fuzzy ffmpeg script generator
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "../TestHelpers.hpp"


int get_start_frame_list(int position, int filter_length)
{
  return (position + 1)*filter_length + 1;
}


int get_start_frame_ffmpeg(int position, int filter_length)
{
  return (position + 1)*filter_length;
}


void verify(int i, int filter_length, int start, int end, int x, int y, int w, int h);


void verify(const std::string& line, int i, int filter_length)
{
  static std::regex r("^delogo=enable='between\\(n,(\\d+),(\\d+)\\)':x=(\\d+):y=(\\d+):w=(\\d+):h=(\\d+),$");

  std::smatch matches;
  if (!std::regex_search(line, matches, r)) {
    BOOST_ERROR("Line " << i << " does not match regexp");
  }

  int start = std::stoi(matches[1].str());
  int end = std::stoi(matches[2].str());
  int x = std::stoi(matches[3].str());
  int y = std::stoi(matches[4].str());
  int w = std::stoi(matches[5].str());
  int h = std::stoi(matches[6].str());
  verify(i, filter_length, start, end, x, y, w, h);
}


void verify_last(const std::string& line, int i, int filter_length)
{
  static std::regex r("^delogo=enable='gte\\(n,(\\d+)\\)':x=(\\d+):y=(\\d+):w=(\\d+):h=(\\d+)$");

  std::smatch matches;
  if (!std::regex_search(line, matches, r)) {
    BOOST_ERROR("Line " << i << " does not match regexp");
  }

  int start = std::stoi(matches[1].str());
  int x = std::stoi(matches[2].str());
  int y = std::stoi(matches[3].str());
  int w = std::stoi(matches[4].str());
  int h = std::stoi(matches[5].str());
  verify(i, filter_length, start, std::numeric_limits<int>::max(), x, y, w, h);
}


void verify(int i, int filter_length, int start, int end, int x, int y, int w, int h)
{
  int regular_start = get_start_frame_ffmpeg(i, filter_length);
  int first_filter_start = get_start_frame_ffmpeg(0, filter_length);
  int regular_end = get_start_frame_ffmpeg(i + 1, filter_length) - 1;

  BOOST_TEST(start <= regular_start);
  BOOST_TEST(start >= first_filter_start);
  BOOST_TEST(end >= regular_end);
  BOOST_TEST(x == i);
  BOOST_TEST(y == i);
  BOOST_TEST(w == i);
  BOOST_TEST(h == i);
}


BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script)
{
  const int iters = 200;
  const int filter_length = 500;

  FilterList list;
  for (int i = 1; i < iters; ++i) {
    list.insert(get_start_frame_list(i, filter_length),
                filter_ptr(new DelogoFilter(i, i, i, i)));
  }
  std::shared_ptr<ScriptGenerator> g = FuzzyScriptGenerator::create(list, 1920, 1080, 25, 2);

  std::stringstream result;
  g->generate_ffmpeg_script(result);

  std::string line;
  std::getline(result, line);
  BOOST_TEST(line == "[0:v]");

  for (int i = 1; i < iters - 1; ++i) {
    std::getline(result, line);
    BOOST_TEST_CONTEXT("line " << i << ": " << line) {
      verify(line, i, filter_length);
    }
  }
  std::getline(result, line);
  BOOST_TEST_CONTEXT("line " << iters - 1 << ": " << line) {
    verify_last(line, iters - 1, filter_length);
  }

  std::getline(result, line);
  BOOST_TEST(line == "[out_v]");
}


BOOST_AUTO_TEST_CASE(cut_positions_should_not_be_changed)
{
  FilterList list;
  list.insert(1, filter_ptr(new DelogoFilter(1, 1, 1, 1)));
  list.insert(501, filter_ptr(new CutFilter()));
  list.insert(1001, filter_ptr(new DelogoFilter(2, 2, 2, 2)));
  std::shared_ptr<ScriptGenerator> g = FuzzyScriptGenerator::create(list, 1920, 1080, 25, 2);

  std::stringstream result;
  g->generate_ffmpeg_script(result);

  std::string line;

  std::getline(result, line);
  BOOST_TEST(line == "[0:v]");

  std::getline(result, line);
  std::regex r1("^delogo=enable='between\\(n,0,\\d+\\)':x=1:y=1:w=1:h=1,$");
  BOOST_TEST(std::regex_search(line, r1));

  std::getline(result, line);
  std::regex r2("^delogo=enable='gte\\(n,\\d+\\)':x=2:y=2:w=2:h=2,$");
  BOOST_TEST(std::regex_search(line, r2));

  std::getline(result, line);
  BOOST_TEST(line == "select='not(between(n,500,999))',setpts=N/FRAME_RATE/TB");

  std::getline(result, line);
  BOOST_TEST(line == "[out_v];");

  std::getline(result, line);
  BOOST_TEST(line == "[0:a]aselect='not(between(t,500/25.000000,999/25.000000))',asetpts=N/SR/TB[out_a]");
}
