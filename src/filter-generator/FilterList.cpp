#include "Filters.hpp"
#include "FilterList.hpp"

using namespace fg;


FilterList::~FilterList()
{
  for (auto& entry: filters_) {
    delete entry.second;
  }
}


void FilterList::insert(int start_frame, Filter* filter)
{
  filters_.insert(std::pair<int, Filter*>(start_frame, filter));
}


std::map<int, Filter*>::size_type FilterList::size() const
{
  return filters_.size();
}
