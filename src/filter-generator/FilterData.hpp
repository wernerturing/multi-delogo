#ifndef FG_FILTER_DATA_H
#define FG_FILTER_DATA_H

#include <string>
#include <istream>
#include <ostream>

#include "FilterList.hpp"


namespace fg {
  class FilterData
  {
  public:
    FilterData();
    FilterData(const std::string& movie_file);

    void set_jump_size(int jump_size);

    std::string movie_file() const;
    int jump_size() const;
    FilterList& filter_list();

    void load(std::istream& in);
    void save(std::ostream& out) const;

  private:
    const static std::string HEADER_;

    std::string movie_file_;
    int jump_size_;
    FilterList filter_list_;
  };
}

#endif // FG_FILTER_DATA_H
