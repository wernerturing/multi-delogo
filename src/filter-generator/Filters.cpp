#include <cstring>
#include <string>

#include "Filters.hpp"

using namespace fg;


Filter::~Filter()
{
}


FilterType NullFilter::type() const
{
  return FilterType::NO_OP;
}


std::string NullFilter::save_str() const
{
  return "none";
}


std::string NullFilter::ffmpeg_str(const std::string& between_expr) const
{
  return "";
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


std::string RectangularFilter::rectangle_ffmpeg_str() const
{
  std::string buf;
  buf.append("x=").append(std::to_string(x_)).push_back(':');
  buf.append("y=").append(std::to_string(y_)).push_back(':');
  buf.append("w=").append(std::to_string(width_)).push_back(':');
  buf.append("h=").append(std::to_string(height_));
  return buf;
}


DelogoFilter::DelogoFilter(int x, int y, int width, int height)
  : RectangularFilter(x, y, width, height)
{
}


FilterType DelogoFilter::type() const
{
  return FilterType::DELOGO;
}


std::string DelogoFilter::save_str() const
{
  std::string buf("delogo;");
  buf.append(rectangle_save_str());
  return buf;
}


std::string DelogoFilter::ffmpeg_str(const std::string& between_expr) const
{
  std::string buf("delogo=");
  buf.append(between_expr).push_back(':');
  buf.append(rectangle_ffmpeg_str());
  return buf;
}


DrawboxFilter::DrawboxFilter(int x, int y, int width, int height)
  : RectangularFilter(x, y, width, height)
{
}


FilterType DrawboxFilter::type() const
{
  return FilterType::DRAWBOX;
}


std::string DrawboxFilter::save_str() const
{
  std::string buf("drawbox;");
  buf.append(rectangle_save_str());
  return buf;
}


std::string DrawboxFilter::ffmpeg_str(const std::string& between_expr) const
{
  std::string buf("drawbox=");
  buf.append(between_expr).push_back(':');
  buf.append(rectangle_ffmpeg_str());
  buf.append(":c=black:t=fill");
  return buf;
}
