#ifndef FILTER_LIST_H
#define FILTER_LIST_H

#include <map>

#include "Filters.hpp"


namespace fg {
  class FilterList
  {
  public:
    typedef std::map<int, Filter*>::size_type size_type;
    typedef std::map<int, Filter*>::const_iterator const_iterator;

    ~FilterList();

    void insert(int start_frame, Filter* filter);
    size_type size() const;
    const_iterator begin() const;
    const_iterator end() const;

    void save(std::ostream& out) const;

  private:
    std::map<int, Filter*> filters_;
  };
}

#endif // FILTER_LIST_H
