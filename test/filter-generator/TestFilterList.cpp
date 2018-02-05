#include "FilterList.hpp"
#include "Filters.hpp"

using namespace fg;


#define BOOST_TEST_MODULE filter list
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "TestHelpers.hpp"

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
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 400);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 1000);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DRAWBOX);
}


BOOST_AUTO_TEST_CASE(insert_should_replace_an_existing_filter) {
  FilterList list;

  NullFilter* filter1 = new NullFilter();
  list.insert(0, filter1);
  DrawboxFilter* filter2 = new DrawboxFilter(1, 2, 3, 4);
  list.insert(500, filter2);

  DelogoFilter* filter3 = new DelogoFilter(10, 15, 100, 20);
  list.insert(500, filter3);

  BOOST_CHECK_EQUAL(list.size(), 2);
  auto it = list.begin();
  BOOST_CHECK_EQUAL(it->first, 0);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::NO_OP);
  ++it;
  BOOST_CHECK_EQUAL(it->first, 500);
  BOOST_CHECK_EQUAL(it->second->type(), FilterType::DELOGO);
}

BOOST_AUTO_TEST_CASE(should_save_the_list) {
  FilterList list;

  list.insert(0, new DelogoFilter(1, 2, 3, 4));
  list.insert(500, new DrawboxFilter(11, 22, 33, 44));
  list.insert(1500, new NullFilter());
  list.insert(1000, new DrawboxFilter(111, 222, 333, 444));

  std::ostringstream out;
  list.save(out);

  std::string expected =
    "0;delogo;1;2;3;4\n"
    "500;drawbox;11;22;33;44\n"
    "1000;drawbox;111;222;333;444\n"
    "1500;none\n";
  BOOST_CHECK_EQUAL(out.str(), expected);
}
