#ifndef FILTERS_H
#define FILTERS_H

#include <string>
#include <ostream>

namespace fg {
  class Filter
  {
  public:
    virtual std::string save_str() const = 0;
  };


  class NullFilter : public Filter
  {
  public:
    virtual std::string save_str() const;
  };
}

#endif // FILTERS_H
