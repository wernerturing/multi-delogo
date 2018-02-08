#include <string>
#include <map>
#include <istream>
#include <ostream>
#include <stdexcept>

#include <boost/optional.hpp>

#include "Exceptions.hpp"
#include "Filters.hpp"
#include "FilterFactory.hpp"
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
  remove(start_frame);
  filters_.insert(std::pair<int, Filter*>(start_frame, filter));
}


void FilterList::remove(int start_frame)
{
  auto iter = filters_.find(start_frame);
  if (iter != filters_.end()) {
    delete iter->second;
    filters_.erase(iter);
  }
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


FilterList::maybe_type FilterList::get_by_start_frame(int start_frame)
{
  auto iter = filters_.find(start_frame);
  if (iter == filters_.end()) {
    return boost::none;
  }

  return boost::make_optional(*iter);
}


void FilterList::load(std::istream& in)
{
  std::string line;
  while (std::getline(in, line)) {
    load_line(line);
  }
}


void FilterList::load_line(const std::string& line)
{
  auto pos = line.find_first_of(';');
  if (pos == std::string::npos) {
    throw InvalidFilterException();
  }

  try {
    int start_frame = std::stoi(line.substr(0, pos));
    Filter* filter = FilterFactory::load(line.substr(pos + 1));

    insert(start_frame, filter);
  } catch (std::invalid_argument& e) {
    throw InvalidFilterException();
  }
}


void FilterList::save(std::ostream& out) const
{
  for (auto& entry: filters_) {
    out << entry.first << ';' << entry.second->save_str() << '\n';
  }
}


void FilterList::generate_ffmpeg_script(std::ostream& out) const
{
  if (filters_.empty()) {
    return;
  }

  generate_ffmpeg_script_intermediary_filters(out);
  generate_ffmpeg_script_last_filter(out);
}


void FilterList::generate_ffmpeg_script_intermediary_filters(std::ostream& out) const
{
  const_iterator i = filters_.begin();

  while (true) {
    auto& current = *i++;
    if (i == filters_.end()) {
      break;
    }

    auto& next = *i;

    std::string frame_expr = get_frame_expression(current.first, next.first);
    std::string ffmpeg_str = current.second->ffmpeg_str(frame_expr);
    if (ffmpeg_str != "") {
      out << ffmpeg_str << ",\n";
    }
  }
}


void FilterList::generate_ffmpeg_script_last_filter(std::ostream& out) const
{
  auto& last = *--filters_.end();
  std::string frame_expr = get_frame_expression(last.first);
  std::string ffmpeg_str = last.second->ffmpeg_str(frame_expr);
  if (ffmpeg_str != "") {
    out << ffmpeg_str << "\n";
  }
}


std::string FilterList::get_frame_expression(int start_frame, int next_start_frame) const
{
  return "enable='between(n," + std::to_string(start_frame)
    + ',' + std::to_string(next_start_frame - 1)
    + ')';
}


std::string FilterList::get_frame_expression(int start_frame) const
{
  return "enable='gte(n," + std::to_string(start_frame) + ')';
}
