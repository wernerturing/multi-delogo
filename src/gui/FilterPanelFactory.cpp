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
#include <stdexcept>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "FilterPanelFactory.hpp"
#include "FilterPanels.hpp"

using namespace mdl;


FilterPanel::FilterPanel()
  : Gtk::Box(Gtk::ORIENTATION_VERTICAL)
{
}


FilterPanel::~FilterPanel()
{
}


bool FilterPanel::creates_filter() const
{
  return true;
}


FilterPanel::type_signal_rectangle_changed FilterPanel::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


FilterPanelFactory::FilterPanelFactory(int frame_width, int frame_height)
  : frame_width_(frame_width)
  , frame_height_(frame_height)
{
}


FilterPanel* FilterPanelFactory::create(fg::Filter* filter)
{
  switch (filter->type()) {
  case fg::FilterType::NO_OP:
    return new FilterPanelNull();

  case fg::FilterType::DELOGO:
    return new FilterPanelDelogo(dynamic_cast<fg::DelogoFilter*>(filter),
                                 frame_width_, frame_height_);

  case fg::FilterType::DRAWBOX:
    return new FilterPanelDrawbox(dynamic_cast<fg::DrawboxFilter*>(filter),
                                  frame_width_, frame_height_);

  default:
    return nullptr;
  }
}


FilterPanel* FilterPanelFactory::create(fg::FilterType type)
{
  switch (type) {
  case fg::FilterType::NO_OP:
    return new FilterPanelNull();

  case fg::FilterType::DELOGO:
    return new FilterPanelDelogo(frame_width_, frame_height_);

  case fg::FilterType::DRAWBOX:
    return new FilterPanelDrawbox(frame_width_, frame_height_);

  default:
    return nullptr;
  }
}


FilterPanel* FilterPanelFactory::convert(fg::Filter* original, fg::FilterType new_type)
{
  if (new_type == fg::FilterType::NO_OP) {
    return create(fg::FilterType::NO_OP);
  }

  if (original->type() == fg::FilterType::NO_OP) {
    return create(new_type);
  }

  throw std::invalid_argument("Unsupported conversion");
}
