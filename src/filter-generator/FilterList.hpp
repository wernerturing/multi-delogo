#ifndef FG_FILTER_LIST_H
#define FG_FILTER_LIST_H

#include <string>
#include <map>
#include <istream>
#include <ostream>

#include <boost/optional.hpp>

#include "Filters.hpp"


namespace fg {
  class FilterList
  {
  public:
    typedef std::map<int, Filter*>::value_type value_type;
    typedef boost::optional<value_type> maybe_type;
    typedef std::map<int, Filter*>::size_type size_type;
    typedef std::map<int, Filter*>::const_iterator const_iterator;

    ~FilterList();

    void insert(int start_frame, Filter* filter);
    void remove(int start_frame);

    size_type size() const;

    const_iterator begin() const;
    const_iterator end() const;
    maybe_type get_by_start_frame(int start_frame);
    maybe_type get_by_position(size_type position);

    void load(std::istream& in);
    void save(std::ostream& out) const;
    void generate_ffmpeg_script(std::ostream& out) const;


  private:
    std::map<int, Filter*> filters_;

    void load_line(const std::string& line);

    void generate_ffmpeg_script_intermediary_filters(std::ostream& out) const;
    void generate_ffmpeg_script_last_filter(std::ostream& out) const;

    std::string get_frame_expression(int start_frame, int next_start_frame) const;
    std::string get_frame_expression(int start_frame) const;
  };
}

#endif // FG_FILTER_LIST_H
