#include <cstring>
#include <string>

#include "Filters.hpp"

using namespace fg;


std::string NullFilter::save_str() const
{
  return "none";
}


RectangularFilter::RectangularFilter(int x, int y, int width, int height)
  : x_(x), y_(y), width_(width), height_(height)
{
}


std::string RectangularFilter::rectangle_save_str() const
{
  std::string buf;
  buf.append(std::to_string(x_)).push_back(';');
  buf.append(std::to_string(y_)).push_back(';');
  buf.append(std::to_string(width_)).push_back(';');
  buf.append(std::to_string(height_));
  return buf;
}


DelogoFilter::DelogoFilter(int x, int y, int width, int height)
  : RectangularFilter(x, y, width, height)
{
}


std::string DelogoFilter::save_str() const
{
  std::string buf("delogo;");
  buf.append(rectangle_save_str());
  return buf;
}


DrawboxFilter::DrawboxFilter(int x, int y, int width, int height)
  : RectangularFilter(x, y, width, height)
{
}


std::string DrawboxFilter::save_str() const
{
  std::string buf("drawbox;");
  buf.append(rectangle_save_str());
  return buf;
}
