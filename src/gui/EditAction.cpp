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
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "EditAction.hpp"
#include "Coordinator.hpp"

using namespace mdl;


RemoveFilterAction::RemoveFilterAction(int start_frame, fg::filter_ptr filter)
  : start_frame_(start_frame)
  , filter_(filter)
{
}


void RemoveFilterAction::execute(Coordinator& coordinator)
{
  printf("Executing RemoveFilter %d\n", start_frame_);
  coordinator.remove_filter(start_frame_);
}


void RemoveFilterAction::undo(Coordinator& coordinator)
{
  printf("Undoing RemoveFilter %d\n", start_frame_);
  // TODO
}
