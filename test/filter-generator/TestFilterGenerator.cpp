#include <sstream>

#include "FilterGenerator.hpp"


#define BOOST_TEST_MODULE filter generator
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_header) {
  fg::FilterData filters("/home/user/videos/test.mp4");
  std::ostringstream out;
  filters.save(out);

  BOOST_CHECK_EQUAL(out.str(), "MDLV1\n/home/user/videos/test.mp4\n");
}
