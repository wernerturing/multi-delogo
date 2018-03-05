/*
 * Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
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
#include <ostream>

#include "ScriptGenerator.hpp"
#include "FilterList.hpp"


namespace fg {
  class RegularScriptGenerator : public ScriptGenerator
  {
  protected:
    RegularScriptGenerator(const FilterList& filter_list);

  public:
    static std::shared_ptr<RegularScriptGenerator> create(const FilterList& filter_list);
    void generate_ffmpeg_script(std::ostream& out) const override;

  protected:
    const FilterList& filter_list_;
    mutable int first_filter_;

    void generate_ffmpeg_script_intermediary_filters(std::ostream& out) const;
    void generate_ffmpeg_script_last_filter(std::ostream& out) const;

    virtual std::string get_frame_expression(int start_frame, int next_start_frame) const;
    virtual std::string get_frame_expression(int start_frame) const;
  };
}

#endif // FG_REGULAR_SCRIPT_GENERATOR_H
