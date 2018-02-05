#ifndef TEST_HELPERS_H
#define TEST_HELPERS_H

#include <boost/version.hpp>

#if BOOST_VERSION >= 106400
#include <ostream>

namespace fg {
  std::ostream& boost_test_print_type(std::ostream& ostr, FilterType const& type)
  {
    switch (type) {
    case FilterType::NO_OP:
      ostr << "NO_OP";
      break;

    case FilterType::DELOGO:
      ostr << "DELOGO";
      break;

    case FilterType::DRAWBOX:
      ostr << "DRAWBOX";
      break;
    }

    return ostr;
  }

#else

BOOST_TEST_DONT_PRINT_LOG_VALUE(fg::FilterType)

#endif

#endif // TEST_HELPERS_H
