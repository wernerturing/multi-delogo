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


  class DelogoFilter : public Filter
  {
  public:
    DelogoFilter(int x, int y, int width, int height);
    virtual std::string save_str() const;

  private:
    int x_;
    int y_;
    int width_;
    int height_;
  };
}

#endif // FILTERS_H
