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
#ifndef FG_FUZZY_SCRIPT_GENERATOR_H
#define FG_FUZZY_SCRIPT_GENERATOR_H

#include <memory>
#include <string>
#include <functional>
#include <random>
#include <ostream>

#include "RegularScriptGenerator.hpp"
#include "FilterList.hpp"


namespace fg {
  class FuzzyScriptGenerator : public RegularScriptGenerator
  {
  protected:
    FuzzyScriptGenerator(const FilterList& filter_list,
                         int frame_width, int frame_height, double fps,
                         double fuzzyness);

  public:
    static std::shared_ptr<FuzzyScriptGenerator> create(const FilterList& filter_list,
                                                        int frame_width, int frame_height, double fps,
                                                        double fuzzyness);
    void generate_ffmpeg_script(std::ostream& out) const override;

  protected:
    std::string get_enable_expression(int start_frame, maybe_int next_start_frame) const override;

  private:
    std::function<double()> rng;
    mutable int first_filter_start_;
    mutable int length_;

    int adjust_start(int start_frame) const;
    maybe_int adjust_end(maybe_int end_frame) const;
  };
}

#endif // FG_FUZZY_SCRIPT_GENERATOR_H
