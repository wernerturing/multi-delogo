#include <cstring>
#include <string>

#include "FilterData.hpp"
#include "FilterList.hpp"

using namespace fg;


const std::string FilterData::HEADER_ = "MDLV1\n";


FilterData::FilterData(const std::string& filename)
  : filename_(filename)
{
}


FilterList& FilterData::filter_list()
{
  return filter_list_;
}


void FilterData::save(std::ostream& out) const
{
  out << HEADER_;
  out << filename_ << '\n';

  filter_list_.save(out);
}

