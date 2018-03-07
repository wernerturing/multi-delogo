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
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "FilterPanels.hpp"

using namespace mdl;


FilterPanelNoParameters::FilterPanelNoParameters()
{
}


FilterPanelNoParameters::MaybeRectangle FilterPanelNoParameters::get_rectangle() const
{
  return boost::none;
}


void FilterPanelNoParameters::set_rectangle(const Rectangle& rect)
{
  // nothing to do
}


bool FilterPanelNoParameters::is_changed() const
{
  return false;
}


void FilterPanelNoParameters::set_changed(bool changed)
{
  // nothing to do
}


FilterPanelNull::FilterPanelNull()
{
}


bool FilterPanelNull::creates_filter() const
{
  return false;
}


fg::Filter* FilterPanelNull::get_filter() const
{
  return new fg::NullFilter();
}


FilterPanelCut::FilterPanelCut()
{
}


fg::Filter* FilterPanelCut::get_filter() const
{
  return new fg::CutFilter();
}


FilterPanelRectangular::FilterPanelRectangular(int frame_width, int frame_height)
  : FilterPanelRectangular(0, 0, 0, 0, frame_width, frame_height)
{
}


FilterPanelRectangular::FilterPanelRectangular(fg::RectangularFilter* filter,
                                               int frame_width, int frame_height)
  : FilterPanelRectangular(filter->x(), filter->y(), filter->width(), filter->height(),
                           frame_width, frame_height)
{
}


FilterPanelRectangular::FilterPanelRectangular(int x, int y, int width, int height,
                                               int frame_width, int frame_height)
  : is_changed_(false)
{
  txt_x_.configure(create_adjustment(x, frame_width - 1), 10, 0);
  txt_y_.configure(create_adjustment(y, frame_height - 1), 10, 0);
  txt_width_.configure(create_adjustment(width, frame_width), 10, 0);
  txt_height_.configure(create_adjustment(height, frame_height), 10, 0);

  Gtk::Grid* grid = Gtk::manage(new Gtk::Grid());
  grid->set_row_spacing(6);
  grid->set_column_spacing(4);

  add_widget(*grid, txt_x_, _("_x:"), 0);
  add_widget(*grid, txt_y_, _("_y:"), 1);
  add_widget(*grid, txt_width_, _("_width:"), 2);
  add_widget(*grid, txt_height_, _("_height:"), 3);

  txt_x_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));
  txt_y_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));
  txt_width_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));
  txt_height_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));

  pack_start(*grid, true, true);
}


Glib::RefPtr<Gtk::Adjustment> FilterPanelRectangular::create_adjustment(int start_value, int max)
{
  return Gtk::Adjustment::create(start_value, 0, max);
}


void FilterPanelRectangular::add_widget(Gtk::Grid& grid, Gtk::Widget& widget,
                                        const Glib::ustring& label, int row)
{
  Gtk::Label* l = Gtk::manage(new Gtk::Label(label, true));
  l->set_mnemonic_widget(widget);
  l->set_halign(Gtk::ALIGN_END);
  grid.attach(*l, 0, row, 1, 1);
  grid.attach_next_to(widget, *l, Gtk::POS_RIGHT, 1, 1);
}


FilterPanelRectangular::MaybeRectangle FilterPanelRectangular::get_rectangle() const
{
  Rectangle rect = {.x = txt_x_.get_value(),
                    .y = txt_y_.get_value(),
                    .width = txt_width_.get_value(),
                    .height = txt_height_.get_value()};
  return boost::make_optional(rect);
}


void FilterPanelRectangular::set_rectangle(const Rectangle& rect)
{
  txt_x_.set_value(rect.x);
  txt_y_.set_value(rect.y);
  txt_width_.set_value(rect.width);
  txt_height_.set_value(rect.height);
}


bool FilterPanelRectangular::is_changed() const
{
  return is_changed_;
}


void FilterPanelRectangular::set_changed(bool changed)
{
  is_changed_ = changed;
}


void FilterPanelRectangular::on_coordinate_change()
{
  is_changed_ = true;
  signal_rectangle_changed_.emit(*get_rectangle());
}


FilterPanelDelogo::FilterPanelDelogo(int frame_width, int frame_height)
  : FilterPanelRectangular(frame_width, frame_height)
{
}


FilterPanelDelogo::FilterPanelDelogo(fg::DelogoFilter* filter,
                                     int frame_width, int frame_height)
  : FilterPanelRectangular(filter, frame_width, frame_height)
{
}


fg::Filter* FilterPanelDelogo::get_filter() const
{
  return new fg::DelogoFilter(txt_x_.get_value_as_int(),
                              txt_y_.get_value_as_int(),
                              txt_width_.get_value_as_int(),
                              txt_height_.get_value_as_int());
}


FilterPanelDrawbox::FilterPanelDrawbox(int frame_width, int frame_height)
  : FilterPanelRectangular(frame_width, frame_height)
{
}


FilterPanelDrawbox::FilterPanelDrawbox(fg::DrawboxFilter* filter,
                                       int frame_width, int frame_height)
  : FilterPanelRectangular(filter, frame_width, frame_height)
{
}


fg::Filter* FilterPanelDrawbox::get_filter() const
{
  return new fg::DrawboxFilter(txt_x_.get_value_as_int(),
                               txt_y_.get_value_as_int(),
                               txt_width_.get_value_as_int(),
                               txt_height_.get_value_as_int());
}
