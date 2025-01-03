/*
 * Copyright (C) 2018-2024 Werner Turing <werner.turing@protonmail.com>
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
#include <cmath>

#include <gtkmm.h>
#include <goocanvas.h>

#include "FrameView.hpp"

namespace mdl {
  bool fv_on_button_press_wrapper(GooCanvasItem* item,
                                  GooCanvasItem* target_item,
                                  GdkEventButton* event,
                                  FrameView* frameview);
  bool fv_on_motion_notify_wrapper(GooCanvasItem* item,
                                   GooCanvasItem* target_item,
                                   GdkEventMotion* event,
                                   FrameView* frameview);
  bool fv_on_button_release_wrapper(GooCanvasItem* item,
                                    GooCanvasItem* target_item,
                                    GdkEventButton* event,
                                    FrameView* frameview);

  bool sr_on_button_press_wrapper(GooCanvasItem* item,
                                  GooCanvasItem* target_item,
                                  GdkEventButton* event,
                                  SelectionRect* rect);
  bool sr_on_motion_notify_wrapper(GooCanvasItem* item,
                                   GooCanvasItem* target_item,
                                   GdkEventMotion* event,
                                   SelectionRect* rect);
  bool sr_on_button_release_wrapper(GooCanvasItem* item,
                                    GooCanvasItem* target_item,
                                    GdkEventButton* event,
                                    SelectionRect* rect);
  bool sr_on_leave_notify_wrapper(GooCanvasItem* item,
                                  GooCanvasItem* target_item,
                                  GdkEventCrossing* event,
                                  SelectionRect* rect);
}


using namespace mdl;


const gdouble SelectionRect::RESIZE_MARGIN_ = 10;


FrameView::FrameView(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& builder,
                     int width, int height,
                     bool can_select_rectangle)
  : Gtk::ScrolledWindow(cobject)
  , drag_(false)
{
  canvas_ = (GooCanvas*) goo_canvas_new();
  goo_canvas_set_bounds(canvas_, 0, 0, width, height);
  g_object_set(canvas_,
               "integer_layout", true,
               "anchor", GOO_CANVAS_ANCHOR_CENTER,
               0);

  auto root = goo_canvas_get_root_item(canvas_);

  image_ = goo_canvas_image_new(root, NULL, 0, 0, NULL);
  if (can_select_rectangle) {
    g_signal_connect(image_, "button-press-event", G_CALLBACK(fv_on_button_press_wrapper), this);
    g_signal_connect(image_, "motion-notify-event", G_CALLBACK(fv_on_motion_notify_wrapper), this);
    g_signal_connect(image_, "button-release-event", G_CALLBACK(fv_on_button_release_wrapper), this);
  }
  goo_canvas_item_add_child(root, image_, -1);

  rect_ = new SelectionRect(100, 50, 150, 30);
  goo_canvas_item_add_child(root, rect_->c_item(), -1);
  if (can_select_rectangle) {
    rect_->enable_drag_and_drop();
    rect_->signal_rectangle_changed().connect(sigc::mem_fun(signal_rectangle_changed_, &type_signal_rectangle_changed::emit));
  }

  temp_rect_ = new SelectionRect();
  goo_canvas_item_add_child(root, temp_rect_->c_item(), -1);

  auto canvas = Glib::wrap((GtkWidget*) canvas_);
  add(*canvas);
}


void FrameView::set_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
  g_object_set(image_, "pixbuf", pixbuf->gobj(), NULL);
}


void FrameView::set_zoom(gdouble level)
{
  goo_canvas_set_scale(canvas_, level);
}


void FrameView::show_rectangle(const Rectangle& rect)
{
  rect_->set_visible(true);
  rect_->set_coordinates(rect);
}


void FrameView::hide_rectangle()
{
  rect_->set_visible(false);
}


void FrameView::scroll_to_current_rectangle()
{
  goo_canvas_scroll_to(canvas_, rect_->get_coordinates().x - 50, rect_->get_coordinates().y - 50);
}


FrameView::type_signal_rectangle_changed FrameView::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


bool FrameView::on_button_press(GooCanvasItem* item, GdkEventButton* event)
{
  if (event->button != 1) {
    return false;
  }

  drag_ = true;
  drag_start_.x = event->x;
  drag_start_.y = event->y;

  goo_canvas_pointer_grab(canvas_, item,
                          (GdkEventMask) (GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_RELEASE_MASK),
                          NULL,
                          event->time);

  return true;
}


bool FrameView::on_motion_notify(GooCanvasItem* item, GdkEventMotion* event)
{
  if (!drag_) {
    return false;
  }

  double width = event->x - drag_start_.x;
  double height = event->y - drag_start_.y;
  if (abs(width) >= 5 || abs(height) >= 5) {
    temp_rect_->set_coordinates({.x = drag_start_.x, .y = drag_start_.y,
                                 .width = width, .height = height});
    temp_rect_->set_visible(true);
  }

  return true;
}


bool FrameView::on_button_release(GooCanvasItem* item, GdkEventButton* event)
{
  if (!drag_) {
    return false;
  }

  drag_ = false;
  goo_canvas_pointer_ungrab(canvas_, item, event->time);
  temp_rect_->set_visible(false);

  Rectangle coordinates = temp_rect_->get_coordinates();
  if (coordinates.width >= 5 || coordinates.height >= 5) {
    rect_->set_coordinates(coordinates);
    rect_->set_visible(true);
    signal_rectangle_changed_.emit(coordinates);
  }

  return true;
}


FrameView::~FrameView()
{
  delete rect_;
  delete temp_rect_;
}


bool mdl::fv_on_button_press_wrapper(GooCanvasItem* item,
                                     GooCanvasItem* target_item,
                                     GdkEventButton* event,
                                     FrameView* frameview)
{
  return frameview->on_button_press(item, event);
}


bool mdl::fv_on_motion_notify_wrapper(GooCanvasItem* item,
                                      GooCanvasItem* target_item,
                                      GdkEventMotion* event,
                                      FrameView* frameview)
{
  return frameview->on_motion_notify(item, event);
}


bool mdl::fv_on_button_release_wrapper(GooCanvasItem* item,
                                       GooCanvasItem* target_item,
                                       GdkEventButton* event,
                                       FrameView* frameview)
{
  return frameview->on_button_release(item, event);
}


SelectionRect::SelectionRect(gdouble x, gdouble y, gdouble width, gdouble height)
  : rect_(goo_canvas_rect_new(NULL, x, y, width, height, NULL))
  , drag_mode_(DragMode::NONE)
{
  GooCanvasLineDash* dashed = goo_canvas_line_dash_new(2, 5.0, 5.0);
  g_object_set(rect_,
               "visibility", GOO_CANVAS_ITEM_HIDDEN,
               "line-width", 1.0,
               "fill-color-rgba", 0x00000060,
               "line-dash", dashed,
               NULL);
}


GooCanvasItem* SelectionRect::c_item()
{
  return rect_;
}


void SelectionRect::set_visible(bool is_visible)
{
  GooCanvasItemVisibility visibility = is_visible
    ? GOO_CANVAS_ITEM_VISIBLE
    : GOO_CANVAS_ITEM_INVISIBLE;
  g_object_set(rect_, "visibility", visibility, NULL);
}


void SelectionRect::enable_drag_and_drop()
{
  g_signal_connect(rect_, "button-press-event", G_CALLBACK(sr_on_button_press_wrapper), this);
  g_signal_connect(rect_, "motion-notify-event", G_CALLBACK(sr_on_motion_notify_wrapper), this);
  g_signal_connect(rect_, "button-release-event", G_CALLBACK(sr_on_button_release_wrapper), this);

  g_signal_connect(rect_, "leave-notify-event", G_CALLBACK(sr_on_leave_notify_wrapper), this);

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


Rectangle SelectionRect::get_coordinates()
{
  Rectangle ret;
  g_object_get(rect_,
               "x", &ret.x,
               "y", &ret.y,
               "width", &ret.width,
               "height", &ret.height,
               NULL);
  return ret;
}


void SelectionRect::set_coordinates(const Rectangle& coordinates)
{
  Rectangle r(normalize(coordinates));
  g_object_set(rect_,
               "x", r.x,
               "y", r.y,
               "width", r.width,
               "height", r.height,
               NULL);
}


SelectionRect::type_signal_rectangle_changed SelectionRect::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


Rectangle SelectionRect::normalize(const Rectangle& original)
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


Point SelectionRect::to_inside_coordinates(const Point& point)
{
  gdouble x, y;
  g_object_get(rect_, "x", &x, "y", &y, NULL);
  return {.x = point.x - x, .y = point.y - y};
}


DragMode SelectionRect::get_drag_mode_for_point(const Point& point)
{
  gdouble width, height;
  g_object_get(rect_, "width", &width, "height", &height, NULL);

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


Glib::RefPtr<Gdk::Cursor> SelectionRect::get_cursor(DragMode mode)
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


Rectangle SelectionRect::get_new_coordinates(const Point& drag_point)
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


bool SelectionRect::on_button_press(GooCanvasItem* item, GdkEventButton* event)
{
  if (event->button != 1) {
    return false;
  }

  start_coordinates_ = get_coordinates();
  drag_start_.x = event->x;
  drag_start_.y = event->y;
  drag_mode_ = get_drag_mode_for_point(to_inside_coordinates(drag_start_));

  GooCanvas* canvas = goo_canvas_item_get_canvas(item);
  goo_canvas_pointer_grab(canvas, item,
                          (GdkEventMask) (GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK | GDK_BUTTON_RELEASE_MASK),
                          get_cursor(drag_mode_)->gobj(),
                          event->time);

  return true;
}


bool SelectionRect::on_motion_notify(GooCanvasItem* item, GdkEventMotion* event)
{
  if (drag_mode_ == DragMode::NONE) {
    Point inside_c = to_inside_coordinates({.x = event->x, .y = event->y});
    DragMode mode = get_drag_mode_for_point(inside_c);
    GooCanvas* canvas = goo_canvas_item_get_canvas(item);
    gdk_window_set_cursor(gtk_widget_get_window(GTK_WIDGET(canvas)),
                          get_cursor(mode)->gobj());

    return false;
  }

  set_coordinates(get_new_coordinates({.x = event->x, .y = event->y}));

  return true;
}


bool SelectionRect::on_button_release(GooCanvasItem* item, GdkEventButton* event)
{
  if (drag_mode_ == DragMode::NONE) {
    return false;
  }

  drag_mode_ = DragMode::NONE;
  GooCanvas* canvas = goo_canvas_item_get_canvas(item);
  goo_canvas_pointer_ungrab(canvas, item, event->time);

  signal_rectangle_changed_.emit(get_coordinates());

  return true;
}


bool SelectionRect::on_leave_notify(GooCanvasItem* item, GdkEventCrossing* event)
{
  GooCanvas* canvas = goo_canvas_item_get_canvas(item);
  GdkWindow* window = gtk_widget_get_window(GTK_WIDGET(canvas));
  gdk_window_set_cursor(window, NULL);
  return false;
}


SelectionRect::~SelectionRect()
{
  g_object_unref(rect_);
}


bool mdl::sr_on_button_press_wrapper(GooCanvasItem* item,
                                     GooCanvasItem* target_item,
                                     GdkEventButton* event,
                                     SelectionRect* rect)
{
  return rect->on_button_press(item, event);
}


bool mdl::sr_on_motion_notify_wrapper(GooCanvasItem* item,
                                      GooCanvasItem* target_item,
                                      GdkEventMotion* event,
                                      SelectionRect* rect)
{
  return rect->on_motion_notify(item, event);
}


bool mdl::sr_on_button_release_wrapper(GooCanvasItem* item,
                                       GooCanvasItem* target_item,
                                       GdkEventButton* event,
                                       SelectionRect* rect)
{
  return rect->on_button_release(item, event);
}


bool mdl::sr_on_leave_notify_wrapper(GooCanvasItem* item,
                                     GooCanvasItem* target_item,
                                     GdkEventCrossing* event,
                                     SelectionRect* rect)
{
  return rect->on_leave_notify(item, event);
}
