/*
 * Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>
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
  class RegularScriptGenerator : public ScriptGenerator
  {
  protected:
    RegularScriptGenerator(const FilterList& filter_list,
                           int frame_width, int frame_height, double fps);

  public:
    static std::shared_ptr<RegularScriptGenerator> create(const FilterList& filter_list,
                                                          int frame_width, int frame_height, double fps);

    bool affects_audio() const override;
    void generate_ffmpeg_script(std::ostream& out) const override;
    int resulting_frames(int original_frames) const override;

  protected:
    const FilterList& filter_list_;
    int frame_width_;
    int frame_height_;
    std::string fps_;
    mutable int first_filter_;

    typedef boost::optional<int> maybe_int;
    mutable std::vector<std::pair<int, maybe_int>> cuts_;

    std::string make_fps_str(double fps);

    void generate_ffmpeg_script_standard_filters(std::ostream& out) const;
    void generate_ffmpeg_script_cuts(std::ostream& out) const;
    void generate_ffmpeg_script_audio(std::ostream& out) const;
    std::string separator() const;

    void process_standard_filter(filter_ptr filter,
                                 int start_frame, maybe_int next_start_frame,
                                 std::ostream& out) const;
    void process_cut_filter(int start_frame, maybe_int next_start_frame) const;

    virtual std::string get_enable_expression(int start_frame, maybe_int next_start_frame) const;
    std::string get_frame_expression(int start_frame, maybe_int next_start_frame) const;
    std::string get_audio_expression(int start_frame, maybe_int next_start_frame) const;
  };
}

#endif // FG_REGULAR_SCRIPT_GENERATOR_H
