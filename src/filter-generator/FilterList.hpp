#ifndef FILTER_LIST_H
#define FILTER_LIST_H

#include <map>

#include "Filters.hpp"


namespace fg {
  class FilterList
  {
  public:
    ~FilterList();

    void insert(int start_frame, Filter* filter);
    std::map<int, Filter*>::size_type size() const;

  private:
    std::map<int, Filter*> filters_;
  };
}

#endif // FILTER_LIST_H
