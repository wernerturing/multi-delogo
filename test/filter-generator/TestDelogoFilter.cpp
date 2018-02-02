#include <string>

#include "Filters.hpp"


#define BOOST_TEST_MODULE delogo filter
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_save_str) {
  fg::DelogoFilter filter(10, 15, 100, 20);

  std::string serialized(filter.save_str());

  BOOST_CHECK_EQUAL(serialized, "delogo;10;15;100;20");
}
