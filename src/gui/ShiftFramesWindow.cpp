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
#include <gtkmm.h>

#include "ShiftFramesWindow.hpp"
#include "MultiDelogoAppWindow.hpp"
#include "FilterListModel.hpp"

using namespace mdl;


ShiftFramesWindow* ShiftFramesWindow::create(const Glib::RefPtr<FilterListModel>& filter_model,
                                             int total_frames, int start_frame)
{
  auto builder = Gtk::Builder::create_from_resource("/wt/multi-delogo/ShiftFramesWindow.ui");
  ShiftFramesWindow* window = nullptr;
  builder->get_widget_derived("shift_frames_window", window,
                              filter_model, total_frames, start_frame);
  return window;
}


ShiftFramesWindow::ShiftFramesWindow(BaseObjectType* cobject,
                                     const Glib::RefPtr<Gtk::Builder>& builder,
                                     const Glib::RefPtr<FilterListModel>& filter_model,
                                     int total_frames, int start_frame)
  : Gtk::Dialog(cobject)
  , filter_model_(filter_model)
  , txt_initial_frame_(nullptr)
  , txt_final_frame_(nullptr)
  , txt_amount_(nullptr)
{
  builder->get_widget("txt_initial_frame", txt_initial_frame_);
  configure_spin(*txt_initial_frame_, total_frames - 1);
  txt_initial_frame_->set_value(start_frame);

  builder->get_widget("txt_final_frame", txt_final_frame_);
  configure_spin(*txt_final_frame_, total_frames);
  txt_final_frame_->set_value(total_frames);

  builder->get_widget("txt_amount", txt_amount_);
  txt_amount_->set_range(-1000, 1000);
  txt_amount_->set_increments(1, 10);
  txt_amount_->set_value(1);
}


void ShiftFramesWindow::configure_spin(Gtk::SpinButton& spin, int max)
{
  spin.set_range(1, max);
  spin.set_increments(1, 10);
}


int ShiftFramesWindow::get_initial_frame() const
{
  return txt_initial_frame_->get_value_as_int();
}


int ShiftFramesWindow::get_final_frame() const
{
  return txt_final_frame_->get_value_as_int();
}


int ShiftFramesWindow::get_amount() const
{
  return txt_amount_->get_value_as_int();
}
