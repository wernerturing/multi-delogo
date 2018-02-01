#ifndef FILTER_GENERATOR_H
#define FILTER_GENERATOR_H

#include <string>
#include <ostream>

namespace fg {
  class FilterData
  {
  public:
    FilterData(const std::string& filename);

    void save(std::ostream& out) const;

  private:
    const static std::string HEADER_;

    std::string filename_;
  };
}

#endif // FILTER_GENERATOR_H
