/*
 * Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
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
#include <algorithm>
#include <cmath>
#include <vector>

#include <gtkmm.h>

#include "FrameView.hpp"


using namespace mdl;


const gdouble SelectionRect::RESIZE_MARGIN_ = 10;


FrameView::FrameView(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& builder,
                     int width, int height,
                     bool can_select_rectangle)
  : Gtk::ScrolledWindow(cobject)
  , image_width_(width)
  , image_height_(height)
  , rect_(new SelectionRect())
  , temp_rect_(new SelectionRect())
  , zoom_(1.0)
  , drag_(false)
{
  if (can_select_rectangle) {
    rect_->create_cursors();
    rect_->signal_rectangle_changed().connect(
      sigc::mem_fun(signal_rectangle_changed_, &type_signal_rectangle_changed::emit));
  }

  canvas_.add_events(Gdk::POINTER_MOTION_MASK | Gdk::LEAVE_NOTIFY_MASK);
  canvas_.signal_draw().connect(sigc::mem_fun(*this, &FrameView::render_canvas));
  if (can_select_rectangle) {
    // In GTK 4 is replaced with GestureClick, same semantics
    gesture_click_ = Gtk::GestureMultiPress::create(canvas_);
    gesture_click_->set_button(GDK_BUTTON_PRIMARY);
    gesture_click_->signal_pressed().connect(sigc::mem_fun(*this, &FrameView::on_canvas_button_press));
    gesture_click_->signal_released().connect(sigc::mem_fun(*this, &FrameView::on_canvas_button_release));

    // Can be simplified if migrated to gtkmm-4: There's a C++ wrapper,
    // no need to use C objects directly
    auto evt_motion = gtk_event_controller_motion_new(GTK_WIDGET(canvas_.gobj()));
    g_signal_connect(evt_motion, "motion",
                     G_CALLBACK(&FrameView::on_canvas_motion_notify_wrapper), this);
    g_signal_connect(evt_motion, "leave",
                     G_CALLBACK(&FrameView::on_canvas_leave_notify_wrapper), this);
  }

  update_canvas_size();
  canvas_.show();
  add(canvas_);
}


FrameView::~FrameView()
{
  delete rect_;
  delete temp_rect_;
}


void FrameView::update_canvas_size()
{
  canvas_.set_size_request(std::max(1, (int) std::lround(image_width_ * zoom_)),
                           std::max(1, (int) std::lround(image_height_ * zoom_)));
}


Point FrameView::content_offset() const
{
  double scaled_width = image_width_ * zoom_;
  double scaled_height = image_height_ * zoom_;
  return {
    std::round(std::max(0.0, (canvas_.get_allocated_width() - scaled_width) / 2.0)),
    std::round(std::max(0.0, (canvas_.get_allocated_height() - scaled_height) / 2.0))
  };
}


Point FrameView::widget_to_image(double x, double y) const
{
  Point offset = content_offset();
  return {.x = (x - offset.x) / zoom_, .y = (y - offset.y) / zoom_};
}


void FrameView::set_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
  if (pixbuf) {
    pixbuf_ = pixbuf;
    canvas_.queue_draw();
  }
}


void FrameView::set_zoom(gdouble level)
{
  zoom_ = level;
  update_canvas_size();
  canvas_.queue_draw();
}


void FrameView::show_rectangle(const Rectangle& rect)
{
  rect_->set_visible(true);
  rect_->set_coordinates(rect);
  canvas_.queue_draw();
}


void FrameView::hide_rectangle()
{
  rect_->set_visible(false);
  canvas_.queue_draw();
}


void FrameView::scroll_to_current_rectangle()
{
  Rectangle c = rect_->get_coordinates();
  Point offset = content_offset();

  auto hadjustment = get_hadjustment();
  auto vadjustment = get_vadjustment();
  if (hadjustment) {
    hadjustment->set_value((c.x - 50) * zoom_ + offset.x);
  }
  if (vadjustment) {
    vadjustment->set_value((c.y - 50) * zoom_ + offset.y);
  }
}


FrameView::type_signal_rectangle_changed FrameView::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


FrameView::type_signal_size_changed FrameView::signal_size_changed()
{
  return signal_size_changed_;
}


void FrameView::on_size_allocate(Gtk::Allocation& allocation)
{
  Gtk::ScrolledWindow::on_size_allocate(allocation);
  signal_size_changed_.emit(allocation.get_width(), allocation.get_height());
}


bool FrameView::render_canvas(const Cairo::RefPtr<Cairo::Context>& cr)
{
  Point offset = content_offset();
  cr->translate(offset.x, offset.y);
  cr->scale(zoom_, zoom_);

  if (pixbuf_) {
    Gdk::Cairo::set_source_pixbuf(cr, pixbuf_, 0, 0);
    cr->paint();
  }

  draw_selection(cr, *rect_);
  draw_selection(cr, *temp_rect_);

  return false;
}


void FrameView::draw_selection(const Cairo::RefPtr<Cairo::Context>& cr, SelectionRect& rect)
{
  if (!rect.is_visible()) {
    return;
  }

  Rectangle r = rect.get_coordinates();

  cr->save();
  cr->rectangle(r.x, r.y, r.width, r.height);
  cr->set_source_rgba(0.0, 0.0, 0.0, 0x60 / 255.0);
  cr->fill_preserve();

  cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
  cr->set_line_width(1.0);
  cr->set_dash(std::vector<double>{5.0, 5.0}, 0.0);
  cr->stroke();
  cr->restore();
}


void FrameView::on_canvas_button_press(int n_press, double x, double y)
{
  Point p = widget_to_image(x, y);

  if (rect_->is_visible() && rect_->contains(p)) {
    rect_->begin_drag(p);
    return;
  }

  drag_ = true;
  drag_start_ = p;
}


void FrameView::on_canvas_button_release(int n_press, double x, double y)
{
  if (rect_->dragging()) {
    rect_->end_drag();
    canvas_.queue_draw();
    return;
  }

  if (!drag_) {
    return;
  }

  drag_ = false;
  temp_rect_->set_visible(false);

  Rectangle coordinates = temp_rect_->get_coordinates();
  if (coordinates.width >= 5 || coordinates.height >= 5) {
    rect_->set_coordinates(coordinates);
    rect_->set_visible(true);
    signal_rectangle_changed_.emit(coordinates);
  }

  canvas_.queue_draw();
}


void FrameView::on_canvas_motion_notify_wrapper(GtkEventControllerMotion* self,
                                                double x,
                                                double y,
                                                FrameView* frameview)
{
  frameview->on_canvas_motion_notify(x, y);
}


void FrameView::on_canvas_motion_notify(double x, double y)
{
  Point p = widget_to_image(x, y);

  if (rect_->dragging()) {
    rect_->update_drag(p);
    canvas_.queue_draw();
    return;
  }

  if (drag_) {
    double width = p.x - drag_start_.x;
    double height = p.y - drag_start_.y;
    if (std::abs(width) >= 5 || std::abs(height) >= 5) {
      temp_rect_->set_coordinates({.x = drag_start_.x, .y = drag_start_.y,
                                   .width = width, .height = height});
      temp_rect_->set_visible(true);
      canvas_.queue_draw();
    }
    return;
  }

  auto window = canvas_.get_window();
  if (window) {
    if (rect_->is_visible() && rect_->contains(p)) {
      window->set_cursor(rect_->cursor_for_point(p));
    } else {
      window->set_cursor();
    }
  }
}


void FrameView::on_canvas_leave_notify_wrapper(GtkEventControllerMotion *self,
                                               FrameView* frameview)
{
  frameview->on_canvas_leave_notify();
}


void FrameView::on_canvas_leave_notify()
{
  auto window = canvas_.get_window();
  if (window) {
    window->set_cursor();
  }
}


SelectionRect::SelectionRect(gdouble x, gdouble y, gdouble width, gdouble height)
  : coordinates_{x, y, width, height}
  , visible_(false)
  , drag_mode_(DragMode::NONE)
{
}


void SelectionRect::set_visible(bool is_visible)
{
  visible_ = is_visible;
}


bool SelectionRect::is_visible() const
{
  return visible_;
}


void SelectionRect::create_cursors()
{
  move_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "move");
  resize_br_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "se-resize");
  resize_bl_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "sw-resize");
  resize_tl_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "nw-resize");
  resize_tr_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "ne-resize");
  resize_b_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "s-resize");
  resize_l_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "w-resize");
  resize_t_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "n-resize");
  resize_r_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), "e-resize");
}


Rectangle SelectionRect::get_coordinates() const
{
  return coordinates_;
}


void SelectionRect::set_coordinates(const Rectangle& coordinates)
{
  coordinates_ = normalize(coordinates);
}


SelectionRect::type_signal_rectangle_changed SelectionRect::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


Rectangle SelectionRect::normalize(const Rectangle& original) const
{
  Rectangle ret(original);
  if (ret.width < 0) {
    ret.width = -ret.width;
    ret.x -= ret.width;
  }
  if (ret.height < 0) {
    ret.height = -ret.height;
    ret.y -= ret.height;
  }
  return ret;
}


Point SelectionRect::to_inside_coordinates(const Point& point) const
{
  return {.x = point.x - coordinates_.x, .y = point.y - coordinates_.y};
}


DragMode SelectionRect::get_drag_mode_for_point(const Point& point) const
{
  gdouble width = coordinates_.width;
  gdouble height = coordinates_.height;

  if (point.x >= width - RESIZE_MARGIN_
      && point.y >= height - RESIZE_MARGIN_) {
    return DragMode::RESIZE_BR;
  } else if (point.x <= RESIZE_MARGIN_
             && point.y >= height - RESIZE_MARGIN_) {
    return DragMode::RESIZE_BL;
  } else if (point.x <= RESIZE_MARGIN_
             && point.y <= RESIZE_MARGIN_) {
    return DragMode::RESIZE_TL;
  } else if (point.x >= width - RESIZE_MARGIN_
             && point.y <= RESIZE_MARGIN_) {
    return DragMode::RESIZE_TR;
  } else if (point.y >= height - RESIZE_MARGIN_) {
    return DragMode::RESIZE_B;
  } else if (point.x <= RESIZE_MARGIN_) {
    return DragMode::RESIZE_L;
  } else if (point.y <= RESIZE_MARGIN_) {
    return DragMode::RESIZE_T;
  } else if (point.x >= width - RESIZE_MARGIN_) {
    return DragMode::RESIZE_R;
  } else {
    return DragMode::MOVE;
  }
}


Glib::RefPtr<Gdk::Cursor> SelectionRect::get_cursor(DragMode mode) const
{
  switch (mode) {
  case DragMode::MOVE:
    return move_cursor_;

  case DragMode::RESIZE_BR:
    return resize_br_cursor_;

  case DragMode::RESIZE_BL:
    return resize_bl_cursor_;

  case DragMode::RESIZE_TL:
    return resize_tl_cursor_;

  case DragMode::RESIZE_TR:
    return resize_tr_cursor_;

  case DragMode::RESIZE_B:
    return resize_b_cursor_;

  case DragMode::RESIZE_L:
    return resize_l_cursor_;

  case DragMode::RESIZE_T:
    return resize_t_cursor_;

  case DragMode::RESIZE_R:
    return resize_r_cursor_;

  default:
    return Glib::RefPtr<Gdk::Cursor>();
  }
}


void SelectionRect::start_drag(DragMode mode, Point start)
{
  drag_mode_ = mode;
  start_coordinates_ = get_coordinates();
  drag_start_ = start;
}


Rectangle SelectionRect::get_new_coordinates(const Point& drag_point) const
{
  Rectangle ret = start_coordinates_;
  gdouble rel_x = drag_point.x - drag_start_.x;
  gdouble rel_y = drag_point.y - drag_start_.y;

  switch (drag_mode_) {
  case DragMode::MOVE:
    ret.x = start_coordinates_.x + rel_x;
    ret.y = start_coordinates_.y + rel_y;
    break;

  case DragMode::RESIZE_BR:
    ret.width = start_coordinates_.width + rel_x;
    ret.height = start_coordinates_.height + rel_y;
    break;

  case DragMode::RESIZE_BL:
    ret.x = start_coordinates_.x + rel_x;
    ret.width = start_coordinates_.width - rel_x;
    ret.height = start_coordinates_.height + rel_y;
    break;

  case DragMode::RESIZE_TL:
    ret.x = start_coordinates_.x + rel_x;
    ret.y = start_coordinates_.y + rel_y;
    ret.width = start_coordinates_.width - rel_x;
    ret.height = start_coordinates_.height - rel_y;
    break;

  case DragMode::RESIZE_TR:
    ret.y = start_coordinates_.y + rel_y;
    ret.width = start_coordinates_.width + rel_x;
    ret.height = start_coordinates_.height - rel_y;
    break;

  case DragMode::RESIZE_B:
    ret.height = start_coordinates_.height + rel_y;
    break;

  case DragMode::RESIZE_L:
    ret.x = start_coordinates_.x + rel_x;
    ret.width = start_coordinates_.width - rel_x;
    break;

  case DragMode::RESIZE_T:
    ret.y = start_coordinates_.y + rel_y;
    ret.height = start_coordinates_.height - rel_y;
    break;

  case DragMode::RESIZE_R:
    ret.width = start_coordinates_.width + rel_x;
    break;

  default:
    break; // Do nothing
  }

  return ret;
}


bool SelectionRect::contains(const Point& point) const
{
  Rectangle r = normalize(coordinates_);
  return point.x >= r.x && point.x <= r.x + r.width
      && point.y >= r.y && point.y <= r.y + r.height;
}


Glib::RefPtr<Gdk::Cursor> SelectionRect::cursor_for_point(const Point& point) const
{
  return get_cursor(get_drag_mode_for_point(to_inside_coordinates(point)));
}


void SelectionRect::begin_drag(const Point& point)
{
  start_drag(get_drag_mode_for_point(to_inside_coordinates(point)), point);
}


void SelectionRect::update_drag(const Point& point)
{
  if (drag_mode_ == DragMode::NONE) {
    return;
  }

  set_coordinates(get_new_coordinates(point));
}


void SelectionRect::end_drag()
{
  if (drag_mode_ == DragMode::NONE) {
    return;
  }

  drag_mode_ = DragMode::NONE;
  signal_rectangle_changed_.emit(get_coordinates());
}


bool SelectionRect::dragging() const
{
  return drag_mode_ != DragMode::NONE;
}
