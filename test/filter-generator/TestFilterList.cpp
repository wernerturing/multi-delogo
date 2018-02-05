#include "FilterList.hpp"
#include "Filters.hpp"

using namespace fg;


#define BOOST_TEST_MODULE filter list
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(insert_should_keep_the_filters_ordered) {
  FilterList list;

  DelogoFilter* filter1 = new DelogoFilter(10, 15, 100, 20);
  list.insert(0, filter1);
  DrawboxFilter* filter2 = new DrawboxFilter(1, 2, 3, 4);
  list.insert(1000, filter2);
  NullFilter* filter3 = new NullFilter();
  list.insert(400, filter3);

  BOOST_CHECK_EQUAL(list.size(), 3);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 0);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 400);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 1000);
}
