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
#include <deque>

#include "EditAction.hpp"
#include "Coordinator.hpp"

using namespace mdl;


UndoManager::UndoManager(Coordinator& coordinator)
  : coordinator_(coordinator)
{
}


void UndoManager::execute_action(edit_action_ptr action)
{
  clear_redo_list();

  add_to_undo_list(action);
  printf("UndoManager: executing action\n");
  action->execute(coordinator_);
}


void UndoManager::add_to_undo_list(edit_action_ptr action)
{
  printf("UndoManager: adding action to undo list\n");
  undo_list_.push_front(action);
}


void UndoManager::clear_redo_list()
{
  printf("UndoManager: clearing redo list\n");
  redo_list_.clear();
}


void UndoManager::undo_last_action()
{
  printf("UndoManager: undo\n");
  edit_action_ptr action = undo_list_.front();
  action->undo(coordinator_);
  move_to_redo_list(action);
}


void UndoManager::move_to_redo_list(edit_action_ptr action)
{
  printf("Moving action to redo list\n");
  redo_list_.push_front(action);
  undo_list_.pop_front();
}


void UndoManager::redo_last_action()
{
  printf("UndoManager: redo\n");
  edit_action_ptr action = redo_list_.front();
  action->execute(coordinator_);
  move_to_undo_list(action);
}


void UndoManager::move_to_undo_list(edit_action_ptr action)
{
  printf("Moving action to undo list\n");
  undo_list_.push_front(action);
  redo_list_.pop_front();
}
