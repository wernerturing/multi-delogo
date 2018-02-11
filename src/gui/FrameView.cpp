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
{
  property_line_width() = 1;
  property_fill_color() = "black";
  GooCanvasLineDash* dashed = goo_canvas_line_dash_new(2, 5.0, 5.0);
  property_line_dash() = Goocanvas::LineDash(dashed, false);

  signal_button_press_event().connect(sigc::mem_fun(*this, &SelectionRect::on_button_press));
  signal_button_release_event().connect(sigc::mem_fun(*this, &SelectionRect::on_button_release));
  signal_motion_notify_event().connect(sigc::mem_fun(*this, &SelectionRect::on_motion_notify));
}


Glib::RefPtr<SelectionRect> SelectionRect::create(gdouble x, gdouble y, gdouble width, gdouble height)
{
  return Glib::RefPtr<SelectionRect>(new SelectionRect(x, y, width, height));
}


bool SelectionRect::on_button_press(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  printf("btn press\n");
  return false;
}


bool SelectionRect::on_button_release(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event)
{
  printf("btn release\n");
  return false;
}


bool SelectionRect::on_motion_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventMotion* event)
{
  printf("motion notify\n");
  return false;
}
