#ifndef FG_FILTER_FACTORY_H
#define FG_FILTER_FACTORY_H

#include <string>

#include "Filters.hpp"

namespace fg {
  class FilterFactory
  {
  public:
    static Filter* load(const std::string& serialized);

  private:
    static Filter* create(const std::string& type, const std::string& parameters);
  };
}

#endif // FG_FILTER_FACTORY_H
