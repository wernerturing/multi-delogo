#ifndef FG_FILTER_DATA_H
#define FG_FILTER_DATA_H

#include <string>
#include <ostream>

#include "FilterList.hpp"


namespace fg {
  class FilterData
  {
  public:
    FilterData(const std::string& filename);

    void set_jump_size(int jump_size);

    FilterList& filter_list();

    void save(std::ostream& out) const;

  private:
    const static std::string HEADER_;

    std::string filename_;
    int jump_size_;
    FilterList filter_list_;
  };
}

#endif // FG_FILTER_DATA_H
