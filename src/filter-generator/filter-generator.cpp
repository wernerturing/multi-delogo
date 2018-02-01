#include <cstring>
#include <string>

#include "filter-generator.hpp"

using namespace fg;


const std::string FilterData::HEADER_ = "MDLV1\n";


FilterData::FilterData(const std::string& filename)
  : filename_(filename)
{
}


void FilterData::save(std::ostream& out) const
{
  out.write(HEADER_.c_str(), HEADER_.length());
  out.write(filename_.c_str(), filename_.length());
  out.put('\n');
}

