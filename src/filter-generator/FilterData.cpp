#include <cstring>
#include <string>

#include "FilterData.hpp"
#include "FilterList.hpp"

using namespace fg;


const std::string FilterData::HEADER_ = "MDLV1\n";


FilterData::FilterData(const std::string& filename)
  : filename_(filename), jump_size_(500)
{
}

void FilterData::set_jump_size(int jump_size)
{
  jump_size_ = jump_size;
}


FilterList& FilterData::filter_list()
{
  return filter_list_;
}


void FilterData::save(std::ostream& out) const
{
  out << HEADER_;
  out << filename_ << '\n';
  out << std::to_string(jump_size_) << '\n';

  filter_list_.save(out);
}

