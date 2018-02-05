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
  auto iter = filters_.find(start_frame);
  if (iter != filters_.end()) {
    delete iter->second;
    filters_.erase(iter);
  }

  filters_.insert(std::pair<int, Filter*>(start_frame, filter));
}


FilterList::size_type FilterList::size() const
{
  return filters_.size();
}


FilterList::const_iterator FilterList::begin() const
{
  return filters_.begin();
}


FilterList::const_iterator FilterList::end() const
{
  return filters_.end();
}
