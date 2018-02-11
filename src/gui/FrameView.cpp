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

  auto root = Goocanvas::GroupModel::create();

  image_ = Goocanvas::ImageModel::create(0, 0);
  root->add_child(image_);

  canvas_.set_root_item_model(root);

  add(canvas_);
}


void FrameView::set_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf)
{
  image_->property_pixbuf().set_value(pixbuf);
}
