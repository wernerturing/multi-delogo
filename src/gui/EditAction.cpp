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
#include <glibmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "EditAction.hpp"
#include "Coordinator.hpp"

using namespace mdl;


ChangeFilterTypeAction::ChangeFilterTypeAction(int start_frame, fg::FilterType old_type, fg::FilterType new_type)
  : start_frame_(start_frame)
  , old_type_(old_type)
  , new_type_(new_type)
{
}


void ChangeFilterTypeAction::execute(Coordinator& coordinator)
{
  printf("Executing ChangeFilterType %d\n", start_frame_);
  coordinator.change_filter_type(start_frame_, new_type_);
}


void ChangeFilterTypeAction::undo(Coordinator& coordinator)
{
  printf("Undoing ChangeFilterType %d\n", start_frame_);
  coordinator.change_filter_type(start_frame_, old_type_);
}


std::string ChangeFilterTypeAction::get_description() const
{
  return Glib::ustring::compose(_("Change filter type at %1"), start_frame_);
}


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
  coordinator.insert_filter(start_frame_, filter_);
}


std::string RemoveFilterAction::get_description() const
{
  return Glib::ustring::compose(_("Remove filter at %1"), start_frame_);
}


ChangeStartFrameAction::ChangeStartFrameAction(int old_start_frame, int new_start_frame, fg::filter_ptr previous_filter)
  : old_start_frame_(old_start_frame)
  , new_start_frame_(new_start_frame)
  , previous_filter_(previous_filter)
{
}


void ChangeStartFrameAction::execute(Coordinator& coordinator)
{
  coordinator.change_start_frame(old_start_frame_, new_start_frame_);
}


void ChangeStartFrameAction::undo(Coordinator& coordinator)
{
  coordinator.change_start_frame(new_start_frame_, old_start_frame_);
  if (previous_filter_) {
    coordinator.insert_filter(new_start_frame_, previous_filter_);
  }
}


std::string ChangeStartFrameAction::get_description() const
{
  return Glib::ustring::compose(_("Change start frame from %1 to %2"), old_start_frame_, new_start_frame_);
}
