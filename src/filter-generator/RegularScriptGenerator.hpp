/*
 * Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
 *
 * This file is part of multi-delogo.
 *
 * multi-delogo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * multi-delogo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef FG_REGULAR_SCRIPT_GENERATOR_H
#define FG_REGULAR_SCRIPT_GENERATOR_H

#include <memory>
#include <string>
#include <utility>
#include <ostream>

#include <boost/optional.hpp>

#include "ScriptGenerator.hpp"
#include "FilterList.hpp"


namespace fg {
  typedef boost::optional<int> maybe_int;

  class RegularScriptGenerator : public ScriptGenerator
  {
  protected:
    RegularScriptGenerator(const FilterList& filter_list,
                           int frame_width, int frame_height, double fps,
                           maybe_int scale_width, maybe_int scale_height,
                           bool no_audio);

  public:
    static std::shared_ptr<RegularScriptGenerator> create(const FilterList& filter_list,
                                                          int frame_width, int frame_height, double fps,
                                                          maybe_int scale_width, maybe_int scale_height,
                                                          bool no_audio);

    void generate_ffmpeg_script(std::ostream& out) const override;
    int resulting_frames(int original_frames) const override;

  protected:
    const FilterList& filter_list_;
    int frame_width_;
    int frame_height_;
    maybe_int scale_width_;
    maybe_int scale_height_;

    mutable int first_filter_;
    mutable std::vector<std::pair<int, maybe_int>> cuts_;

    int generate_filter_segments(std::ostream& out) const;
    void generate_segment(std::ostream& out, int segment, filter_ptr filter,
                          int start_frame, maybe_int next_start_frame) const;
    bool first_filter_does_not_start_at_first_frame(int start_frame) const;
    void copy_first_segment_unchanged(std::ostream& out, int next_start) const;
    std::string generate_trim(int start_frame, maybe_int next_start_frame) const;
    std::string generate_atrim(int start_frame, maybe_int next_start_frame) const;
    void generate_final_concat(std::ostream& out, int n_segments) const;
  };
}

#endif // FG_REGULAR_SCRIPT_GENERATOR_H
