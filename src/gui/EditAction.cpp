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
#include <utility>

#include <glibmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "EditAction.hpp"
#include "Coordinator.hpp"

using namespace mdl;


AddFilterAction::AddFilterAction(int start_frame, fg::filter_ptr filter)
  : start_frame_(start_frame)
  , filter_(filter)
{
}


void AddFilterAction::execute(Coordinator& coordinator)
{
  coordinator.insert_filter(start_frame_, filter_);
}


void AddFilterAction::undo(Coordinator& coordinator)
{
  coordinator.remove_filter(start_frame_);
}


std::string AddFilterAction::get_description() const
{
  return Glib::ustring::compose(_("Add filter at %1"), start_frame_);
}


UpdateFilterAction::UpdateFilterAction(int start_frame, fg::filter_ptr old_filter, fg::filter_ptr new_filter)
  : start_frame_(start_frame)
  , old_filter_(old_filter)
  , new_filter_(new_filter)
{
}


void UpdateFilterAction::execute(Coordinator& coordinator)
{
  coordinator.update_filter(start_frame_, new_filter_);
}


void UpdateFilterAction::undo(Coordinator& coordinator)
{
  coordinator.update_filter(start_frame_, old_filter_);
}


std::string UpdateFilterAction::get_description() const
{
  return Glib::ustring::compose(_("Update filter at %1"), start_frame_);
}


ChangeFilterTypeAction::ChangeFilterTypeAction(int start_frame, fg::filter_ptr old_filter, fg::filter_ptr new_filter)
  : UpdateFilterAction(start_frame, old_filter, new_filter)
{
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
  coordinator.remove_filter(start_frame_);
}


void RemoveFilterAction::undo(Coordinator& coordinator)
{
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


ShiftAction::ShiftAction(int start, int end, int amount)
  : start_(start)
  , end_(end)
  , amount_(amount)
{
}


void ShiftAction::execute(Coordinator& coordinator)
{
  std::pair<int, int> frames = coordinator.shift(start_, end_, amount_);
  actual_start_ = frames.first;
  actual_end_ = frames.second;
}


void ShiftAction::undo(Coordinator& coordinator)
{
  coordinator.shift(actual_start_ + amount_, actual_end_ + amount_, -amount_);
}


std::string ShiftAction::get_description() const
{
  return Glib::ustring::compose(_("Shift starting frames by %1 from %2 to %3"),
                                amount_, start_, end_);
}
