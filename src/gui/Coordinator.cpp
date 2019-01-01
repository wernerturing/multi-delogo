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

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterFactory.hpp"
#include "filter-generator/Filters.hpp"

#include "Coordinator.hpp"
#include "FilterList.hpp"
#include "FrameNavigator.hpp"
#include "FrameView.hpp"
#include "FilterPanelFactory.hpp"
#include "ShiftFramesWindow.hpp"
#include "EditAction.hpp"
#include "Utils.hpp"

using namespace mdl;


Coordinator::Coordinator(Gtk::Window& parent_window,
                         int number_of_frames, int frame_width, int frame_height)
  : undo_manager_(*this)
  , parent_window_(parent_window)
  , frame_navigator_(nullptr)
  , frame_view_(nullptr)
  , number_of_frames_(number_of_frames)
  , panel_factory_(number_of_frames, frame_width, frame_height)
  , current_filter_panel_(nullptr)
  , current_filter_(nullptr)
  , scroll_filter_(true)
{
}


void Coordinator::set_undo_buttons(Gtk::Widget* btn_undo, Gtk::Widget* btn_redo)
{
  undo_manager_.set_undo_buttons(btn_undo, btn_redo);
}


void Coordinator::set_filter_list(FilterList* filter_list)
{
  filter_list_ = filter_list;
  filter_model_ = filter_list->get_model();

  on_filter_selected_ = filter_list_->signal_selection_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_filter_selected));

  filter_list_->signal_previous_filter().connect(
    sigc::mem_fun(*this, &Coordinator::on_previous_filter));
  filter_list_->signal_next_filter().connect(
    sigc::mem_fun(*this, &Coordinator::on_next_filter));

  filter_list_->signal_remove_filter().connect(
    sigc::mem_fun(*this, &Coordinator::on_remove_filter));

  filter_list_->signal_shift().connect(
    sigc::mem_fun(*this, &Coordinator::on_shift));

  on_filter_type_changed_ = filter_list_->signal_type_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_filter_type_changed));
}


void Coordinator::set_frame_navigator(FrameNavigator* frame_navigator)
{
  frame_navigator_ = frame_navigator;
  frame_view_ = frame_navigator_->get_frame_view();

  frame_navigator_->signal_frame_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_frame_changed));

  on_frame_rectangle_changed_ = frame_view_->signal_rectangle_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_frame_rectangle_changed));

  frame_navigator_->change_displayed_frame(1);
}


void Coordinator::on_previous_filter()
{
  auto iter = filter_model_->get_for_frame(current_frame_ - 1);
  if (!iter) {
    return;
  }

  frame_navigator_->change_displayed_frame((*iter)[filter_model_->columns.start_frame]);
}


void Coordinator::on_next_filter()
{
  auto iter = filter_model_->get_for_frame(current_frame_);
  if (iter) {
    ++iter;
  } else {
    iter = filter_model_->children().begin();
  }

  if (!iter) {
    return;
  }

  frame_navigator_->change_displayed_frame((*iter)[filter_model_->columns.start_frame]);
}


void Coordinator::set_scroll_filter(bool state)
{
  scroll_filter_ = state;
}


int Coordinator::get_current_frame()
{
  return current_frame_;
}


void Coordinator::on_undo()
{
  undo_manager_.undo_last_action();
}


void Coordinator::on_redo()
{
  undo_manager_.redo_last_action();
}


void Coordinator::on_filter_selected(int start_frame)
{
  frame_navigator_->change_displayed_frame(start_frame);
}


void Coordinator::on_frame_changed(int new_frame)
{
  auto iter = filter_model_->get_for_frame(new_frame);

  if (iter && (*iter)[filter_model_->columns.start_frame] == new_frame) {
    select_row(iter);
  } else {
    unselect_rows();
  }

  change_displayed_filter(iter);

  current_frame_ = new_frame;
}


void Coordinator::select_row(const FilterListModel::iterator& iter)
{
  on_filter_selected_.block();
  filter_list_->select(iter);
  on_filter_selected_.block(false);

  filter_list_->scroll_to_row(iter);
}


void Coordinator::unselect_rows()
{
  on_filter_selected_.block();
  filter_list_->unselect();
  on_filter_selected_.block(false);
}


void Coordinator::change_displayed_filter(const FilterListModel::iterator& iter)
{
  if (!iter) {
    return;
  }

  int start_frame = (*iter)[filter_model_->columns.start_frame];
  fg::filter_ptr filter = (*iter)[filter_model_->columns.filter];
  if (filter == current_filter_) {
    return;
  }
  current_filter_ = filter;
  current_filter_start_frame_ = start_frame;

  update_displayed_panel(filter->type(), panel_factory_.create(start_frame, filter));

  auto rect = current_filter_panel_->get_rectangle();
  if (rect) {
    frame_view_->show_rectangle(*rect);
    if (scroll_filter_) {
      frame_view_->scroll_to_current_rectangle();
    }
  } else {
    frame_view_->hide_rectangle();
  }
}


void Coordinator::update_displayed_panel(fg::FilterType type, FilterPanel* panel)
{
  current_filter_panel_ = Gtk::manage(panel);

  on_filter_type_changed_.block();
  filter_list_->set_filter(type, current_filter_panel_);
  on_filter_type_changed_.block(false);

  on_panel_rectangle_changed_ = current_filter_panel_->signal_rectangle_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_panel_rectangle_changed));
  on_start_frame_changed_ = current_filter_panel_->signal_start_frame_changed().connect(
    sigc::mem_fun(*this, &Coordinator::on_start_frame_changed));
}


bool Coordinator::displaying_filter_start_frame()
{
  auto iter = filter_model_->get_by_start_frame(current_frame_);
  return bool(iter);
}


void Coordinator::on_filter_type_changed(fg::FilterType new_type)
{
  if (!current_filter_) {
    return;
  }

  auto new_filter = fg::FilterFactory::convert(current_filter_, new_type);
  if (displaying_filter_start_frame()) {
    edit_action_ptr action = edit_action_ptr(new ChangeFilterTypeAction(current_frame_, current_filter_, new_filter));
    undo_manager_.execute_action(action);
  } else {
    edit_action_ptr action = edit_action_ptr(new AddFilterAction(current_frame_, new_filter));
    undo_manager_.execute_action(action);
  }
}


void Coordinator::on_frame_rectangle_changed(Rectangle rect)
{
  if (!current_filter_panel_) {
    create_new_filter_panel();
  }

  on_panel_rectangle_changed_.block();
  current_filter_panel_->set_rectangle(rect);
  on_panel_rectangle_changed_.block(false);

  update_filter_for_current_frame();
}


void Coordinator::on_panel_rectangle_changed(Rectangle rect)
{
  on_frame_rectangle_changed_.block();
  frame_view_->show_rectangle(rect);
  on_frame_rectangle_changed_.block(false);

  update_filter_for_current_frame();
}


void Coordinator::update_filter_for_current_frame()
{
  if (!current_filter_panel_->creates_filter()) {
    return;
  }

  if (displaying_filter_start_frame()) {
    update_current_filter();
  } else {
    add_new_filter_for_current_frame();
  }
}


void Coordinator::update_current_filter()
{
  auto new_filter = current_filter_panel_->get_filter();
  edit_action_ptr action = edit_action_ptr(new UpdateFilterAction(current_frame_, current_filter_, new_filter));
  undo_manager_.execute_action(action);
}


void Coordinator::add_new_filter_for_current_frame()
{
  bool saved_scroll_to_filter = scroll_filter_;
  scroll_filter_ = false;

  auto new_filter = current_filter_panel_->get_filter();
  edit_action_ptr action = edit_action_ptr(new AddFilterAction(current_frame_, new_filter));
  undo_manager_.execute_action(action);

  scroll_filter_ = saved_scroll_to_filter;
}


void Coordinator::on_start_frame_changed(int start_frame)
{
  if (!confirm_overwrite_by_start_frame_change(start_frame)) {
    set_start_frame_in_filter_panel(current_filter_start_frame_);
    return;
  }

  fg::filter_ptr previous_filter;
  auto previous_filter_iter = filter_model_->get_by_start_frame(start_frame);
  if (previous_filter_iter) {
    previous_filter = (*previous_filter_iter)[filter_model_->columns.filter];
  }

  edit_action_ptr action = edit_action_ptr(new ChangeStartFrameAction(current_filter_start_frame_, start_frame, previous_filter));
  undo_manager_.execute_action(action);
}


void Coordinator::set_start_frame_in_filter_panel(int start_frame)
{
  on_start_frame_changed_.block();
  current_filter_panel_->set_start_frame(start_frame);
  // For some reason must set to be called in the future.
  // Calling block(false) directly causes a signal to be emitted.
  Glib::signal_idle().connect([&] {
      on_start_frame_changed_.block(false);
      return false;
    });
}


bool Coordinator::confirm_overwrite_by_start_frame_change(int start_frame)
{
  if (!filter_model_->get_by_start_frame(start_frame)) {
    return true;
  }

  return confirmation_dialog(parent_window_,
                             Glib::ustring::compose(_("Overwrite filter starting at frame %1?"), start_frame),
                             _("_Overwrite"), _("_Don't overwrite"));
}


void Coordinator::create_new_filter_panel()
{
  fg::FilterType filter_type = filter_list_->get_selected_type();
  update_displayed_panel(filter_type, panel_factory_.create(current_frame_, filter_type));
}


void Coordinator::on_remove_filter()
{
  auto iter = filter_list_->get_selected();
  fg::filter_ptr filter = (*iter)[filter_model_->columns.filter];

  edit_action_ptr remove_action = edit_action_ptr(new RemoveFilterAction(current_frame_, filter));
  undo_manager_.execute_action(remove_action);
}


void Coordinator::remove_filter(int start_frame)
{
  auto iter = filter_model_->get_by_start_frame(start_frame);
  on_filter_selected_.block();
  filter_model_->remove(iter);
  on_filter_selected_.block(false);
  on_frame_changed(current_frame_);
}


void Coordinator::insert_filter(int start_frame, fg::filter_ptr filter)
{
  filter_model_->insert(start_frame, filter);
  current_filter_panel_->set_changed(false);
  frame_navigator_->change_displayed_frame(start_frame);
}


void Coordinator::update_filter(int start_frame, fg::filter_ptr filter)
{
  auto iter = filter_model_->get_by_start_frame(start_frame);
  (*iter)[filter_model_->columns.filter] = filter;

  bool saved_scroll_to_filter = scroll_filter_;
  scroll_filter_ = false;
  frame_navigator_->change_displayed_frame(start_frame);
  scroll_filter_ = saved_scroll_to_filter;
}


void Coordinator::change_start_frame(int old_start_frame, int new_start_frame)
{
  auto iter = filter_model_->get_by_start_frame(old_start_frame);

  on_filter_selected_.block();
  (*iter)[filter_model_->columns.start_frame] = new_start_frame;
  on_filter_selected_.block(false);
  unselect_rows();

  set_start_frame_in_filter_panel(new_start_frame);

  current_filter_start_frame_ = new_start_frame;
}


void Coordinator::on_shift()
{
  ShiftFramesWindow* window = ShiftFramesWindow::create(filter_model_, number_of_frames_, current_frame_);
  window->set_transient_for(parent_window_);

  if (window->run() == GTK_RESPONSE_ACCEPT) {
    int start = window->get_initial_frame();
    int end = window->get_final_frame();
    int amount = window->get_amount();
    edit_action_ptr shift_action = edit_action_ptr(new ShiftAction(start, end, amount));
    undo_manager_.execute_action(shift_action);
  }

  delete window;
}


std::pair<int, int> Coordinator::shift(int start, int end, int amount)
{
  on_filter_selected_.block();
  std::pair<int, int> r = filter_model_->shift_frames(start, end, amount);
  on_filter_selected_.block(false);

  on_frame_changed(current_frame_);

  return r;
}
