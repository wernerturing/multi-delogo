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
#include <gtkmm.h>
#include <goocanvasmm.h>

#include "FrameView.hpp"

using namespace mdl;


const gdouble SelectionRect::RESIZE_MARGIN_ = 10;


FrameView::FrameView(int width, int height)
  : Gtk::ScrolledWindow()
  , drag_(false)
{
  canvas_.set_bounds(0, 0, width, height);
  canvas_.property_integer_layout() = true;
  canvas_.property_anchor() = Goocanvas::ANCHOR_CENTER;

  auto root = canvas_.get_root_item();

  image_ = Goocanvas::Image::create(0, 0);
  image_->signal_button_press_event().connect(sigc::mem_fun(*this, &FrameView::on_button_press));
  image_->signal_motion_notify_event().connect(sigc::mem_fun(*this, &FrameView::on_motion_notify));
  image_->signal_button_release_event().connect(sigc::mem_fun(*this, &FrameView::on_button_release));
  root->add_child(image_);

  rect_ = SelectionRect::create(100, 50, 150, 30);
  rect_->enable_drag_and_drop();
  root->add_child(rect_);
  rect_->signal_rectangle_changed().connect(sigc::mem_fun(signal_rectangle_changed_, &type_signal_rectangle_changed::emit));

  temp_rect_ = SelectionRect::create();
  root->add_child(temp_rect_);

  add(canvas_);
}


void FrameView::set_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
  image_->property_pixbuf() = pixbuf;
}


void FrameView::set_zoom(int level)
{
  canvas_.set_scale(level / 100.0);
}

void FrameView::show_rectangle(const Rectangle& rect)
{
  rect_->property_visibility() = Goocanvas::ITEM_VISIBLE;
  rect_->set_coordinates(rect);
}


void FrameView::hide_rectangle()
{
  rect_->property_visibility() = Goocanvas::ITEM_HIDDEN;
}


FrameView::type_signal_rectangle_changed FrameView::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


bool FrameView::on_button_press(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  if (event->button != 1) {
    return false;
  }

  drag_ = true;
  temp_rect_->set_coordinates({.x = event->x, .y = event->y,
                               .width = 1, .height = 1});
  temp_rect_->property_visibility() = Goocanvas::ITEM_VISIBLE;
  drag_start_.x = event->x;
  drag_start_.y = event->y;

  item->get_canvas()->pointer_grab(item,
                                   Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_RELEASE_MASK,
                                   event->time);

  return true;
}


bool FrameView::on_motion_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventMotion* event)
{
  if (!drag_) {
    return false;
  }

  temp_rect_->set_coordinates({.x = drag_start_.x, .y = drag_start_.y,
                               .width = event->x - drag_start_.x,
                               .height = event->y - drag_start_.y});
  return true;
}


bool FrameView::on_button_release(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  if (!drag_) {
    return false;
  }

  drag_ = false;
  item->get_canvas()->pointer_ungrab(item, event->time);

  rect_->set_coordinates(temp_rect_->get_coordinates());
  rect_->property_visibility() = Goocanvas::ITEM_VISIBLE;
  temp_rect_->property_visibility() = Goocanvas::ITEM_HIDDEN;

  signal_rectangle_changed_.emit(temp_rect_->get_coordinates());

  return true;
}


SelectionRect::SelectionRect(gdouble x, gdouble y, gdouble width, gdouble height)
  : Goocanvas::Rect(x, y, width, height)
  , drag_mode_(DragMode::NONE)
{
  property_visibility() = Goocanvas::ITEM_HIDDEN;

  property_line_width() = 1;
  property_fill_color_rgba() = 0x00000060;
  GooCanvasLineDash* dashed = goo_canvas_line_dash_new(2, 5.0, 5.0);
  property_line_dash() = Goocanvas::LineDash(dashed, false);
}


void SelectionRect::enable_drag_and_drop()
{
  signal_button_press_event().connect(sigc::mem_fun(*this, &SelectionRect::on_button_press));
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &SelectionRect::on_motion_notify));
  signal_button_release_event().connect(sigc::mem_fun(*this, &SelectionRect::on_button_release));

  signal_leave_notify_event().connect(sigc::mem_fun(*this, &SelectionRect::on_leave_notify));

  move_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), Gdk::FLEUR);
  resize_br_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), Gdk::BOTTOM_RIGHT_CORNER);
}


Glib::RefPtr<SelectionRect> SelectionRect::create(gdouble x, gdouble y, gdouble width, gdouble height)
{
  return Glib::RefPtr<SelectionRect>(new SelectionRect(x, y, width, height));
}


Rectangle SelectionRect::get_coordinates()
{
  Rectangle ret;
  ret.x = property_x();
  ret.y = property_y();
  ret.width = property_width();
  ret.height = property_height();
  return ret;
}


void SelectionRect::set_coordinates(Rectangle coordinates)
{
  property_x() = coordinates.x;
  property_y() = coordinates.y;
  property_width() = coordinates.width;
  property_height() = coordinates.height;
}


SelectionRect::type_signal_rectangle_changed SelectionRect::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


Point SelectionRect::to_inside_coordinates(const Point& point)
{
  return {.x = point.x - property_x(), .y = point.y - property_y()};
}


DragMode SelectionRect::get_drag_mode_for_point(const Point& point)
{
  if (point.x >= property_width() - RESIZE_MARGIN_
      && point.y >= property_height() - RESIZE_MARGIN_) {
    return DragMode::RESIZE_BR;
  }

  return DragMode::MOVE;
}


Glib::RefPtr<Gdk::Cursor> SelectionRect::get_cursor(DragMode mode)
{
  switch (mode) {
  case DragMode::MOVE:
    return move_cursor_;

  case DragMode::RESIZE_BR:
    return resize_br_cursor_;

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


Rectangle SelectionRect::get_new_coordinates(Point drag_point)
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

  default:
    break; // Do nothing
  }

  return ret;
}


bool SelectionRect::on_button_press(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  if (event->button != 1) {
    return false;
  }

  start_coordinates_ = get_coordinates();
  drag_start_.x = event->x;
  drag_start_.y = event->y;
  drag_mode_ = get_drag_mode_for_point(to_inside_coordinates(drag_start_));

  item->get_canvas()->pointer_grab(item,
                                   Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_RELEASE_MASK,
                                   get_cursor(drag_mode_),
                                   event->time);

  return true;
}


bool SelectionRect::on_motion_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventMotion* event)
{
  if (drag_mode_ == DragMode::NONE) {
    Point inside_c = to_inside_coordinates({.x = event->x, .y = event->y});
    DragMode mode = get_drag_mode_for_point(inside_c);
    item->get_canvas()->get_window()->set_cursor(get_cursor(mode));

    return false;
  }

  set_coordinates(get_new_coordinates({.x = event->x, .y = event->y}));

  return true;
}


bool SelectionRect::on_button_release(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  if (drag_mode_ == DragMode::NONE) {
    return false;
  }

  drag_mode_ = DragMode::NONE;
  item->get_canvas()->pointer_ungrab(item, event->time);

  signal_rectangle_changed_.emit(get_coordinates());

  return true;
}


bool SelectionRect::on_leave_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventCrossing* event)
{
  item->get_canvas()->get_window()->set_cursor();
  return false;
}
