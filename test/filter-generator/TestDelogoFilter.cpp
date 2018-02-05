#include <string>

#include "Filters.hpp"


#define BOOST_TEST_MODULE delogo filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_type) {
  fg::DelogoFilter filter(1, 2, 3, 4);

  BOOST_CHECK_EQUAL(filter.type(), fg::FilterType::DELOGO);
}

BOOST_AUTO_TEST_CASE(test_save_str) {
  fg::DelogoFilter filter(10, 15, 100, 20);

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "delogo;10;15;100;20");
}

BOOST_AUTO_TEST_CASE(test_ffmpeg_str) {
  fg::DelogoFilter filter(50, 60, 150, 30);

  std::string ffmpeg(filter.ffmpeg_str("<BETWEEN>"));

  BOOST_CHECK_EQUAL(ffmpeg, "delogo=<BETWEEN>:x=50:y=60:w=150:h=30");
}
