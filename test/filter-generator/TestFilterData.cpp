#include <sstream>

#include "FilterData.hpp"
#include "Filters.hpp"

using namespace fg;


#define BOOST_TEST_MODULE filter generator
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_save) {
  FilterData filters("/home/user/videos/test.mp4");
  filters.filter_list().insert(0, new DelogoFilter(1, 2, 3, 4));
  filters.filter_list().insert(250, new DelogoFilter(9, 8, 7, 6));

  std::ostringstream out;
  filters.save(out);

  std::string expected =
    "MDLV1\n"
    "/home/user/videos/test.mp4\n"
    "0;delogo;1;2;3;4\n"
    "250;delogo;9;8;7;6\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}
