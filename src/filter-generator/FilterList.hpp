#ifndef FILTER_LIST_H
#define FILTER_LIST_H

#include <string>
#include <map>

#include "Filters.hpp"


namespace fg {
  class FilterList
  {
  public:
    typedef std::map<int, Filter*>::size_type size_type;
    typedef std::map<int, Filter*>::const_iterator const_iterator;

    ~FilterList();

    void insert(int start_frame, Filter* filter);
    size_type size() const;
    const_iterator begin() const;
    const_iterator end() const;

    void save(std::ostream& out) const;
    void generate_ffmpeg_script(std::ostream& out) const;


  private:
    std::map<int, Filter*> filters_;

    void generate_ffmpeg_script_intermediary_filters(std::ostream& out) const;
    void generate_ffmpeg_script_last_filter(std::ostream& out) const;

    std::string get_frame_expression(int start_frame, int next_start_frame) const;
    std::string get_frame_expression(int start_frame) const;
  };
}

#endif // FILTER_LIST_H
