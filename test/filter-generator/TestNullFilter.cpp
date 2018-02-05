#include <string>

#include "Filters.hpp"


#define BOOST_TEST_MODULE null filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_type) {
  fg::NullFilter filter;

  BOOST_CHECK_EQUAL(filter.type(), fg::FilterType::NO_OP);
}

BOOST_AUTO_TEST_CASE(test_save_str) {
  fg::NullFilter filter;

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "none");
}

BOOST_AUTO_TEST_CASE(test_ffmpeg_str) {
  fg::NullFilter filter;

  std::string ffmpeg(filter.ffmpeg_str("<BETWEEN>"));

  BOOST_CHECK_EQUAL(ffmpeg, "");
}
