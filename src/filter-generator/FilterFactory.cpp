#include <string>
#include <stdexcept>

#include "Filters.hpp"
#include "FilterFactory.hpp"
#include "Exceptions.hpp"

using namespace fg;


Filter* FilterFactory::load(const std::string& serialized)
{
  auto pos = serialized.find_first_of(';');
  if (pos == std::string::npos) {
    throw InvalidFilterException();
  }

  std::string type = serialized.substr(0, pos);
  std::string parameters = serialized.substr(pos + 1);

  return create(type, parameters);
}


Filter* FilterFactory::create(const std::string& type, const std::string& parameters)
{
  if (type == "none") {
    return NullFilter::load(parameters);
  } else if (type == "delogo") {
    return DelogoFilter::load(parameters);
  } else if (type == "drawbox") {
    return DrawboxFilter::load(parameters);
  } else {
    throw UnknownFilterException();
  }
}
