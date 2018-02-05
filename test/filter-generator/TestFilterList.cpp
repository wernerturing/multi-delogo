#include "FilterList.hpp"
#include "Filters.hpp"

using namespace fg;


#define BOOST_TEST_MODULE filter list
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(insert_should_keep_the_filters_ordered) {
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


BOOST_AUTO_TEST_CASE(insert_should_replace_an_existing_filter) {
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

BOOST_AUTO_TEST_CASE(should_save_the_list) {
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
    "1500;none\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}

BOOST_AUTO_TEST_CASE(should_generate_ffmpeg_script) {
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

BOOST_AUTO_TEST_CASE(should_discard_a_null_filter_at_the_end) {
  FilterList list;

  list.insert(0, new DelogoFilter(10, 11, 12, 13));
  list.insert(1000, new NullFilter());

  std::ostringstream out;
  list.generate_ffmpeg_script(out);

  std::string expected =
    "delogo=enable='between(n,0,999):x=10:y=11:w=12:h=13,\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}
