#include <string>
#include <stdexcept>

#include "Filters.hpp"
#include "Exceptions.hpp"

using namespace fg;


Filter::~Filter()
{
}


NullFilter* NullFilter::load(const std::string& parameters)
{
  if (parameters != "") {
    throw InvalidParametersException();
  }

  return new NullFilter();
}


FilterType NullFilter::type() const
{
  return FilterType::NO_OP;
}


std::string NullFilter::save_str() const
{
  return "none;";
}


std::string NullFilter::ffmpeg_str(const std::string& between_expr) const
{
  return "";
}


RectangularFilter::RectangularFilter(int x, int y, int width, int height)
  : x_(x), y_(y), width_(width), height_(height)
{
}


void RectangularFilter::load_rectangle(const std::string& parameters,
                                       int& x, int& y, int& width, int& height)
{
  std::vector<std::string> dimensions = split(parameters, ';');
  if (dimensions.size() != 4) {
    throw InvalidParametersException();
  }

  try {
    x      = std::stoi(dimensions[0]);
    y      = std::stoi(dimensions[1]);
    width  = std::stoi(dimensions[2]);
    height = std::stoi(dimensions[3]);
  } catch (std::invalid_argument& e) {
    throw InvalidParametersException();
  }
}


std::vector<std::string> RectangularFilter::split(const std::string& text, char sep)
{
  std::vector<std::string> tokens;
  std::size_t start = 0, end = 0;

  while ((end = text.find(sep, start)) != std::string::npos) {
    tokens.push_back(text.substr(start, end - start));
    start = end + 1;
  }
  tokens.push_back(text.substr(start));

  return tokens;
}


int RectangularFilter::x() const
{
  return x_;
}


int RectangularFilter::y() const
{
  return y_;
}


int RectangularFilter::width() const
{
  return width_;
}


int RectangularFilter::height() const
{
  return height_;
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


DelogoFilter* DelogoFilter::load(const std::string& parameters)
{
  int x, y, width, height;

  load_rectangle(parameters, x, y, width, height);
  return new DelogoFilter(x, y, width, height);
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


DrawboxFilter* DrawboxFilter::load(const std::string& parameters)
{
  int x, y, width, height;

  load_rectangle(parameters, x, y, width, height);
  return new DrawboxFilter(x, y, width, height);
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
