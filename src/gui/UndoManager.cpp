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
#include <deque>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "EditAction.hpp"
#include "Coordinator.hpp"

using namespace mdl;


UndoManager::UndoManager(Coordinator& coordinator)
  : coordinator_(coordinator)
{
}


void UndoManager::set_undo_buttons(Gtk::Widget* btn_undo, Gtk::Widget* btn_redo)
{
  btn_undo_ = btn_undo;
  btn_redo_ = btn_redo;

  btn_undo_->set_sensitive(false);
  btn_redo_->set_sensitive(false);
}


void UndoManager::execute_action(edit_action_ptr action)
{
  clear_redo_list();

  add_to_undo_list(action);
  action->execute(coordinator_);
}


void UndoManager::add_to_undo_list(edit_action_ptr action)
{
  if (undo_list_.size() == UNDO_SIZE_) {
    undo_list_.pop_back();
  }

  undo_list_.push_front(action);

  update_buttons();
}


void UndoManager::clear_redo_list()
{
  redo_list_.clear();
}


void UndoManager::undo_last_action()
{
  if (undo_list_.empty()) {
    return;
  }

  edit_action_ptr action = undo_list_.front();
  action->undo(coordinator_);
  move_to_redo_list(action);
}


void UndoManager::move_to_redo_list(edit_action_ptr action)
{
  redo_list_.push_front(action);
  undo_list_.pop_front();
  update_buttons();
}


void UndoManager::redo_last_action()
{
  if (redo_list_.empty()) {
    return;
  }

  edit_action_ptr action = redo_list_.front();
  action->execute(coordinator_);
  move_to_undo_list(action);
}


void UndoManager::move_to_undo_list(edit_action_ptr action)
{
  undo_list_.push_front(action);
  redo_list_.pop_front();
  update_buttons();
}


void UndoManager::update_buttons()
{
  update_button(btn_undo_, undo_list_, _("Undo \"%1\""));
  update_button(btn_redo_, redo_list_, _("Redo \"%1\""));
}


void UndoManager::update_button(Gtk::Widget* button, std::deque<edit_action_ptr>& list, const std::string base_text)
{
  if (list.empty()) {
    button->set_sensitive(false);
    button->set_tooltip_text("");
  } else {
    button->set_sensitive(true);
    edit_action_ptr action = list.front();
    button->set_tooltip_text(Glib::ustring::compose(base_text, action->get_description()));
  }
}
