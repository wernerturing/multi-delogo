#include <string>

#include "Filters.hpp"


#define BOOST_TEST_MODULE drawbox filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_save_str) {
  fg::DrawboxFilter filter(10, 15, 100, 20);

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "drawbox;10;15;100;20");
}

BOOST_AUTO_TEST_CASE(test_ffmpeg_str) {
  fg::DrawboxFilter filter(50, 60, 150, 30);

  std::string ffmpeg(filter.ffmpeg_str("<BETWEEN>"));

  BOOST_CHECK_EQUAL(ffmpeg, "drawbox=<BETWEEN>:x=50:y=60:w=150:h=30:c=black:t=fill");
}
