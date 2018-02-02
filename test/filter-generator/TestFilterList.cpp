#include "FilterList.hpp"
#include "Filters.hpp"


#define BOOST_TEST_MODULE filter list
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_insert) {
  fg::FilterList list;

  fg::DelogoFilter* filter = new fg::DelogoFilter(10, 15, 100, 20);
  list.insert(0, filter);

  BOOST_CHECK_EQUAL(list.size(), 1);
}
