#include <string>

#include "Filters.hpp"
#include "Exceptions.hpp"


#define BOOST_TEST_MODULE delogo filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_construction) {
  fg::DelogoFilter filter(90, 80, 70, 60);

  BOOST_CHECK_EQUAL(filter.type(), fg::FilterType::DELOGO);
  BOOST_CHECK_EQUAL(filter.x(), 90);
  BOOST_CHECK_EQUAL(filter.y(), 80);
  BOOST_CHECK_EQUAL(filter.width(), 70);
  BOOST_CHECK_EQUAL(filter.height(), 60);
}

BOOST_AUTO_TEST_CASE(test_load)
{
  fg::DelogoFilter* filter = fg::DelogoFilter::load("1;22;333;4444");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::DELOGO);
  BOOST_CHECK_EQUAL(filter->x(), 1);
  BOOST_CHECK_EQUAL(filter->y(), 22);
  BOOST_CHECK_EQUAL(filter->width(), 333);
  BOOST_CHECK_EQUAL(filter->height(), 4444);
}

BOOST_AUTO_TEST_CASE(test_load_with_too_little_params)
{
  BOOST_CHECK_THROW(fg::DelogoFilter::load("1;22;333"),
                    fg::InvalidParametersException);
}

BOOST_AUTO_TEST_CASE(test_load_with_too_many_params)
{
  BOOST_CHECK_THROW(fg::DelogoFilter::load("1;22;333;4444;5555"),
                    fg::InvalidParametersException);
}

BOOST_AUTO_TEST_CASE(test_load_with_invalid_parameter)
{
  BOOST_CHECK_THROW(fg::DelogoFilter::load("1;22;abc;4444"),
                    fg::InvalidParametersException);
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
