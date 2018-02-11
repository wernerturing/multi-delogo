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
{
  canvas_.set_bounds(0, 0, width, height);
  canvas_.property_integer_layout() = true;

  auto root = canvas_.get_root_item();

  image_ = Goocanvas::Image::create(0, 0);
  root->add_child(image_);

  rect_ = SelectionRect::create(100, 50, 150, 30);
  root->add_child(rect_);

  add(canvas_);
}


void FrameView::set_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
  image_->property_pixbuf() = pixbuf;
}


SelectionRect::SelectionRect(gdouble x, gdouble y, gdouble width, gdouble height)
  : Goocanvas::Rect(x, y, width, height)
  , drag_mode_(DragMode::NONE)
{
  property_line_width() = 1;
  property_fill_color_rgba() = 0x00000060;
  GooCanvasLineDash* dashed = goo_canvas_line_dash_new(2, 5.0, 5.0);
  property_line_dash() = Goocanvas::LineDash(dashed, false);

  signal_button_press_event().connect(sigc::mem_fun(*this, &SelectionRect::on_button_press));
  signal_button_release_event().connect(sigc::mem_fun(*this, &SelectionRect::on_button_release));
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &SelectionRect::on_motion_notify));

  signal_leave_notify_event().connect(sigc::mem_fun(*this, &SelectionRect::on_leave_notify));

  move_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), Gdk::FLEUR);
  resize_br_cursor_ = Gdk::Cursor::create(Gdk::Display::get_default(), Gdk::BOTTOM_RIGHT_CORNER);
}


Glib::RefPtr<SelectionRect> SelectionRect::create(gdouble x, gdouble y, gdouble width, gdouble height)
{
  return Glib::RefPtr<SelectionRect>(new SelectionRect(x, y, width, height));
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


bool SelectionRect::on_button_press(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  if (event->button != 1) {
    return false;
  }

  drag_mode_ = DragMode::MOVE;
  start_x_ = item->property_x();
  start_y_ = item->property_y();
  drag_x_ = event->x;
  drag_y_ = event->y;

  item->get_canvas()->pointer_grab(item,
                                   Gdk::POINTER_MOTION_MASK | Gdk::POINTER_MOTION_HINT_MASK | Gdk::BUTTON_RELEASE_MASK,
                                   event->time);

  return true;
}


bool SelectionRect::on_button_release(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  if (drag_mode_ == DragMode::NONE) {
    return false;
  }

  drag_mode_ = DragMode::NONE;
  item->get_canvas()->pointer_ungrab(item, event->time);

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

  gdouble rel_x = event->x - drag_x_;
  gdouble rel_y = event->y - drag_y_;
  item->property_x() = start_x_ + rel_x;
  item->property_y() = start_y_ + rel_y;

  return true;
}


bool SelectionRect::on_leave_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventCrossing* event)
{
  item->get_canvas()->get_window()->set_cursor();
  return false;
}
