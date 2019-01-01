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
#include <memory>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "FilterPanelFactory.hpp"
#include "FilterPanels.hpp"

using namespace mdl;


FilterPanelFactory::FilterPanelFactory(int max_frame, int frame_width, int frame_height)
  : max_frame_(max_frame)
  , frame_width_(frame_width)
  , frame_height_(frame_height)
{
}


FilterPanel* FilterPanelFactory::create(int start_frame, fg::filter_ptr filter)
{
  switch (filter->type()) {
  case fg::FilterType::NO_OP:
    return new FilterPanelNull(start_frame, max_frame_);

  case fg::FilterType::CUT:
    return new FilterPanelCut(start_frame, max_frame_);

  case fg::FilterType::DELOGO:
    return new FilterPanelDelogo(start_frame, max_frame_,
                                 std::dynamic_pointer_cast<fg::DelogoFilter>(filter),
                                 frame_width_, frame_height_);

  case fg::FilterType::DRAWBOX:
    return new FilterPanelDrawbox(start_frame, max_frame_,
                                  std::dynamic_pointer_cast<fg::DrawboxFilter>(filter),
                                  frame_width_, frame_height_);

  case fg::FilterType::REVIEW:
    return new FilterPanelReview(start_frame, max_frame_);

  default:
    return nullptr;
  }
}


FilterPanel* FilterPanelFactory::create(int start_frame, fg::FilterType type)
{
  switch (type) {
  case fg::FilterType::NO_OP:
    return new FilterPanelNull(start_frame, max_frame_);

  case fg::FilterType::CUT:
    return new FilterPanelCut(start_frame, max_frame_);

  case fg::FilterType::DELOGO:
    return new FilterPanelDelogo(start_frame, max_frame_,
                                 frame_width_, frame_height_);

  case fg::FilterType::DRAWBOX:
    return new FilterPanelDrawbox(start_frame, max_frame_,
                                  frame_width_, frame_height_);

  case fg::FilterType::REVIEW:
    return new FilterPanelReview(start_frame, max_frame_);

  default:
    return nullptr;
  }
}
