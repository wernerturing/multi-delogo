#include <string>

#include "Filters.hpp"
#include "Exceptions.hpp"


#define BOOST_TEST_MODULE drawbox filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(test_construction) {
  fg::DrawboxFilter filter(1, 2, 3, 4);

  BOOST_CHECK_EQUAL(filter.type(), fg::FilterType::DRAWBOX);
  BOOST_CHECK_EQUAL(filter.x(), 1);
  BOOST_CHECK_EQUAL(filter.y(), 2);
  BOOST_CHECK_EQUAL(filter.width(), 3);
  BOOST_CHECK_EQUAL(filter.height(), 4);
}

BOOST_AUTO_TEST_CASE(test_load)
{
  fg::DrawboxFilter* filter = fg::DrawboxFilter::load("50;60;100;80");

  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::DRAWBOX);
  BOOST_CHECK_EQUAL(filter->x(), 50);
  BOOST_CHECK_EQUAL(filter->y(), 60);
  BOOST_CHECK_EQUAL(filter->width(), 100);
  BOOST_CHECK_EQUAL(filter->height(), 80);
}

BOOST_AUTO_TEST_CASE(test_load_with_too_little_params)
{
  BOOST_CHECK_THROW(fg::DrawboxFilter::load("1;22"),
                    fg::InvalidParametersException);
}

BOOST_AUTO_TEST_CASE(test_load_with_too_many_params)
{
  BOOST_CHECK_THROW(fg::DrawboxFilter::load("1;2;3;4;5;6"),
                    fg::InvalidParametersException);
}

BOOST_AUTO_TEST_CASE(test_load_with_invalid_parameter)
{
  BOOST_CHECK_THROW(fg::DrawboxFilter::load("1;fd;2;4444"),
                    fg::InvalidParametersException);
}

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
