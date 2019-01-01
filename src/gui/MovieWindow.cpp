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

#include "common/FrameProvider.hpp"

#include "filter-generator/FilterData.hpp"

#include "MovieWindow.hpp"
#include "FilterList.hpp"
#include "FrameNavigator.hpp"
#include "Coordinator.hpp"
#include "MultiDelogoApp.hpp"
#include "FindLogosWindow.hpp"
#include "EncodeWindow.hpp"

using namespace mdl;


MovieWindow* MovieWindow::create(const std::string& project_file,
                                 std::unique_ptr<fg::FilterData> filter_data,
                                 const Glib::RefPtr<FrameProvider>& frame_provider,
                                 Gtk::Application& app)
{
  auto builder = Gtk::Builder::create_from_resource("/wt/multi-delogo/MovieWindow.ui");
  MovieWindow* window = nullptr;
  builder->get_widget_derived("movie_window", window,
                              project_file, std::move(filter_data),
                              frame_provider, app);
  return window;
}


MovieWindow::MovieWindow(BaseObjectType* cobject,
                         const Glib::RefPtr<Gtk::Builder>& builder,
                         const std::string& project_file,
                         std::unique_ptr<fg::FilterData> filter_data,
                         const Glib::RefPtr<FrameProvider>& frame_provider,
                         Gtk::Application& app)
  : MultiDelogoAppWindow(cobject)
  , btn_undo_(nullptr)
  , btn_redo_(nullptr)
  , project_file_(project_file)
  , filter_data_(std::move(filter_data))
  , filter_list_(nullptr)
  , frame_navigator_(nullptr)
  , coordinator_(*this, frame_provider->get_number_of_frames(), frame_provider->get_frame_width(), frame_provider->get_frame_height())
{
  set_title(Glib::ustring::compose("multi-delogo: %1",
                                   Glib::path_get_basename(project_file)));

  configure_toolbar(builder, app);
  coordinator_.set_undo_buttons(btn_undo_, btn_redo_);

  builder->get_widget_derived("filter_list", filter_list_, filter_data_->filter_list());
  coordinator_.set_filter_list(filter_list_);

  builder->get_widget_derived("frame_navigator", frame_navigator_,
                              *this, frame_provider);
  frame_navigator_->set_jump_size(filter_data_->jump_size());
  coordinator_.set_frame_navigator(frame_navigator_);

  signal_key_press_event().connect(sigc::mem_fun(*this, &MovieWindow::on_key_press));
}


void MovieWindow::configure_toolbar(const Glib::RefPtr<Gtk::Builder>& builder,
                                    Gtk::Application& app)
{
  Gtk::ToolButton* btn_new = nullptr;
  builder->get_widget("btn_new", btn_new);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_new->gobj()), MultiDelogoApp::ACTION_NEW.c_str());

  Gtk::ToolButton* btn_open = nullptr;
  builder->get_widget("btn_open", btn_open);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_open->gobj()), MultiDelogoApp::ACTION_OPEN.c_str());

  add_action("save", sigc::mem_fun(*this, &MovieWindow::on_save));
  Gtk::ToolButton* btn_save = nullptr;
  builder->get_widget("btn_save", btn_save);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_save->gobj()), "win.save");

  add_action("undo", sigc::mem_fun(coordinator_, &Coordinator::on_undo));
  app.set_accels_for_action("win.undo", {"<Ctrl>z"});
  builder->get_widget("btn_undo", btn_undo_);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_undo_->gobj()), "win.undo");

  add_action("redo", sigc::mem_fun(coordinator_, &Coordinator::on_redo));
  app.set_accels_for_action("win.redo", {"<Ctrl><Shift>z", "<Ctrl>y"});
  builder->get_widget("btn_redo", btn_redo_);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_redo_->gobj()), "win.redo");

  Gtk::ToggleToolButton* chk_scroll_filter = nullptr;
  builder->get_widget("chk_scroll_filter", chk_scroll_filter);
  chk_scroll_filter->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &MovieWindow::on_scroll_filter_toggled),
               chk_scroll_filter));

  Gtk::RadioMenuItem* chk_prev_frame_no = nullptr;
  builder->get_widget("chk_prev_frame_no", chk_prev_frame_no);
  chk_prev_frame_no->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &MovieWindow::on_set_prev_frame),
               chk_prev_frame_no, FrameNavigator::PrevFrame::NO));

  Gtk::RadioMenuItem* chk_prev_frame_fit = nullptr;
  builder->get_widget("chk_prev_frame_fit", chk_prev_frame_fit);
  chk_prev_frame_fit->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &MovieWindow::on_set_prev_frame),
               chk_prev_frame_fit, FrameNavigator::PrevFrame::FIT));

  Gtk::RadioMenuItem* chk_prev_frame_same = nullptr;
  builder->get_widget("chk_prev_frame_same", chk_prev_frame_same);
  chk_prev_frame_same->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &MovieWindow::on_set_prev_frame),
               chk_prev_frame_same, FrameNavigator::PrevFrame::SAME));

  add_action("find-logos", sigc::mem_fun(*this, &MovieWindow::on_find_logos));
  Gtk::ToolButton* btn_find_logos = nullptr;
  builder->get_widget("btn_find_logos", btn_find_logos);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_find_logos->gobj()), "win.find-logos");

  add_action("encode", sigc::mem_fun(*this, &MovieWindow::on_encode));
  Gtk::ToolButton* btn_encode = nullptr;
  builder->get_widget("btn_encode", btn_encode);
  gtk_actionable_set_action_name(GTK_ACTIONABLE(btn_encode->gobj()), "win.encode");
}


bool MovieWindow::on_key_press(GdkEventKey* key_event)
{
  switch (key_event->keyval) {
  case GDK_KEY_A:
  case GDK_KEY_a:
    frame_navigator_->jump_step_frame(-1);
    return true;

  case GDK_KEY_S:
  case GDK_KEY_s:
    frame_navigator_->single_step_frame(-1);
    return true;

  case GDK_KEY_D:
  case GDK_KEY_d:
    frame_navigator_->single_step_frame(1);
    return true;

  case GDK_KEY_F:
  case GDK_KEY_f:
    frame_navigator_->jump_step_frame(1);
    return true;

  case GDK_KEY_C:
  case GDK_KEY_c:
    coordinator_.on_previous_filter();
    return true;

  case GDK_KEY_V:
  case GDK_KEY_v:
    coordinator_.on_next_filter();
    return true;
  }

  return false;
}


void MovieWindow::on_save()
{
  filter_data_->set_jump_size(frame_navigator_->get_jump_size());
  get_application()->save_project(project_file_, *filter_data_);
}


void MovieWindow::on_find_logos()
{
  FindLogosWindow* window
    = FindLogosWindow::create(*filter_data_,
                              frame_navigator_->get_number_of_frames(),
                              coordinator_.get_current_frame(),
                              frame_navigator_->get_jump_size(),
                              get_application()->is_verbose());
  window->set_transient_for(*this);
  window->set_modal();
  window->signal_hide().connect(sigc::mem_fun(*filter_list_, &FilterList::refresh_list));

  get_application()->register_window(window);
}


void MovieWindow::on_encode()
{
  if (filter_data_->filter_list().empty()) {
    Gtk::MessageDialog dlg(*this, _("There are no filters. Please define at least one filter before encoding."), false, Gtk::MESSAGE_ERROR);
    dlg.run();
    return;
  }

  if (filter_data_->filter_list().has_review_filter()) {
    Gtk::MessageDialog dlg(*this, _("Encoding cannot be done when there are 'review' filters. Please change them to some other filter (such as 'none'), or remove them."), false, Gtk::MESSAGE_ERROR);
    dlg.run();
    return;
  }

  on_save();

  EncodeWindow* window = EncodeWindow::create(std::move(filter_data_),
                                              frame_navigator_->get_frame_width(), frame_navigator_->get_frame_height(),
                                              frame_navigator_->get_number_of_frames(),
                                              frame_navigator_->get_fps());
  get_application()->register_window(window);

  hide();
}


void MovieWindow::on_scroll_filter_toggled(Gtk::ToggleToolButton* chk)
{
  coordinator_.set_scroll_filter(chk->get_active());
}


void MovieWindow::on_set_prev_frame(Gtk::RadioMenuItem* radio, FrameNavigator::PrevFrame setting)
{
  // The signal is emitted for the item that is unchecked and for the item that is checked, so we ignore unchecking signals
  if (!radio->get_active()) {
    return;
  }

  frame_navigator_->set_show_prev_frame(setting);
}


void MovieWindow::on_hide()
{
  // When this is called because of on_encode there is no filter_data_ anymore
  if (filter_data_) {
    on_save();
  }
}
