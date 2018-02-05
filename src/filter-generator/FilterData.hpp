#ifndef FILTER_GENERATOR_H
#define FILTER_GENERATOR_H

#include <string>
#include <ostream>

#include "FilterList.hpp"


namespace fg {
  class FilterData
  {
  public:
    FilterData(const std::string& filename);

    FilterList& filter_list();

    void save(std::ostream& out) const;

  private:
    const static std::string HEADER_;

    std::string filename_;
    FilterList filter_list_;
  };
}

#endif // FILTER_GENERATOR_H
