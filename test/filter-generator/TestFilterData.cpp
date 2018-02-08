#include <sstream>

#include "Exceptions.hpp"
#include "FilterData.hpp"
#include "Filters.hpp"

using namespace fg;


#define BOOST_TEST_MODULE filter data
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "TestHelpers.hpp"

BOOST_AUTO_TEST_CASE(load_should_fail_if_header_is_invalid)
{
  std::istringstream in(
    "MDLV2\n"
    "file=Movie.mp4\n");

  FilterData filters;
  BOOST_CHECK_THROW(filters.load(in), InvalidFilterDataException);
}


BOOST_AUTO_TEST_CASE(load_should_fail_if_movie_file_is_missing)
{
  std::istringstream in("MDLV1\n");

  FilterData filters;
  BOOST_CHECK_THROW(filters.load(in), InvalidFilterDataException);
}


BOOST_AUTO_TEST_CASE(load_should_fail_if_jump_size_is_missing)
{
  std::istringstream in(
    "MDLV1\n"
    "Movie.mp4\n");

  FilterData filters;
  BOOST_CHECK_THROW(filters.load(in), InvalidFilterDataException);
}


BOOST_AUTO_TEST_CASE(load_should_fail_if_jump_size_is_invalid)
{
  std::istringstream in(
    "MDLV1\n"
    "Movie.mp4\n"
    "abc\n");

  FilterData filters;
  BOOST_CHECK_THROW(filters.load(in), InvalidFilterDataException);
}


BOOST_AUTO_TEST_CASE(a_file_with_no_filters_is_valid)
{
  std::istringstream in(
    "MDLV1\n"
    "Movie.mp4\n"
    "500\n");

  FilterData filters;
  filters.load(in);

  BOOST_CHECK_EQUAL(filters.movie_file(), "Movie.mp4");
  BOOST_CHECK_EQUAL(filters.jump_size(), 500);
  BOOST_CHECK_EQUAL(filters.filter_list().size(), 0);
}


BOOST_AUTO_TEST_CASE(should_load_a_file_with_filters)
{
  std::istringstream in(
    "MDLV1\n"
    "Movie.mp4\n"
    "500\n"
    "0;drawbox;10;20;30;40\n"
    "600;delogo;100;50;200;80\n");

  FilterData filters;
  filters.load(in);

  BOOST_CHECK_EQUAL(filters.movie_file(), "Movie.mp4");
  BOOST_CHECK_EQUAL(filters.jump_size(), 500);

  BOOST_CHECK_EQUAL(filters.filter_list().size(), 2);
  auto it = filters.filter_list().begin();
  BOOST_CHECK_EQUAL(it->first, 0);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 600);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}


BOOST_AUTO_TEST_CASE(should_fail_if_some_filter_is_invalid)
{
  std::istringstream in(
    "MDLV1\n"
    "Movie.mp4\n"
    "500\n"
    "0;blur;10;20;30;40\n"
    "600;delogo;100;50;200;80\n");

  FilterData filters;
  BOOST_CHECK_THROW(filters.load(in), UnknownFilterException);
}


BOOST_AUTO_TEST_CASE(test_save)
{
  FilterData filters("/home/user/videos/test.mp4");
  filters.set_jump_size(360);
  filters.filter_list().insert(0, new DelogoFilter(1, 2, 3, 4));
  filters.filter_list().insert(250, new DelogoFilter(9, 8, 7, 6));

  std::ostringstream out;
  filters.save(out);

  std::string expected =
    "MDLV1\n"
    "/home/user/videos/test.mp4\n"
    "360\n"
    "0;delogo;1;2;3;4\n"
    "250;delogo;9;8;7;6\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}
