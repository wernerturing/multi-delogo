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
#include <glibmm/i18n.h>

#include "filter-generator/Filters.hpp"

#include "FilterPanels.hpp"

using namespace mdl;


FilterPanel::FilterPanel(int start_frame, int max_frame)
{
  set_orientation(Gtk::ORIENTATION_VERTICAL);
  set_row_spacing(6);
  set_column_spacing(4);

  lbl_start_frame_.set_label(_("_Start frame:"));
  lbl_start_frame_.set_use_underline();
  lbl_start_frame_.set_mnemonic_widget(txt_start_frame_);

  txt_start_frame_.configure(Gtk::Adjustment::create(start_frame, 1, max_frame), 10, 0);
  txt_start_frame_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanel::on_start_frame_change));

  lbl_start_frame_.set_halign(Gtk::ALIGN_END);
  attach(lbl_start_frame_, 0, -1, 1, 1);
  attach_next_to(txt_start_frame_, lbl_start_frame_, Gtk::POS_RIGHT, 1, 1);
}


FilterPanel::~FilterPanel()
{
}


bool FilterPanel::creates_filter() const
{
  return true;
}


void FilterPanel::set_start_frame(int start_frame)
{
  txt_start_frame_.set_value(start_frame);
}


FilterPanel::type_signal_start_frame_changed FilterPanel::signal_start_frame_changed()
{
  return signal_start_frame_changed_;
}


FilterPanel::type_signal_rectangle_changed FilterPanel::signal_rectangle_changed()
{
  return signal_rectangle_changed_;
}


void FilterPanel::on_start_frame_change()
{
  signal_start_frame_changed_.emit(txt_start_frame_.get_value_as_int());
}


FilterPanelNoParameters::FilterPanelNoParameters(int start_frame, int max_frame)
  : FilterPanel(start_frame, max_frame)
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


FilterPanelNull::FilterPanelNull(int start_frame, int max_frame)
  : FilterPanelNoParameters(start_frame, max_frame)
{
}


bool FilterPanelNull::creates_filter() const
{
  return false;
}


fg::filter_ptr FilterPanelNull::get_filter() const
{
  return fg::filter_ptr(new fg::NullFilter());
}


FilterPanelCut::FilterPanelCut(int start_frame, int max_frame)
  : FilterPanelNoParameters(start_frame, max_frame)
{
}


fg::filter_ptr FilterPanelCut::get_filter() const
{
  return fg::filter_ptr(new fg::CutFilter());
}


FilterPanelReview::FilterPanelReview(int start_frame, int max_frame)
  : FilterPanelNoParameters(start_frame, max_frame)
{
}


fg::filter_ptr FilterPanelReview::get_filter() const
{
  return fg::filter_ptr(new fg::ReviewFilter());
}


FilterPanelRectangular::FilterPanelRectangular(int start_frame, int max_frame,
                                               int frame_width, int frame_height)
  : FilterPanelRectangular(start_frame, max_frame,
                           0, 0, 0, 0,
                           frame_width, frame_height)
{
}


FilterPanelRectangular::FilterPanelRectangular(int start_frame, int max_frame,
                                               std::shared_ptr<fg::RectangularFilter> filter,
                                               int frame_width, int frame_height)
  : FilterPanelRectangular(start_frame, max_frame,
                           filter->x(), filter->y(), filter->width(), filter->height(),
                           frame_width, frame_height)
{
}


FilterPanelRectangular::FilterPanelRectangular(int start_frame, int max_frame,
                                               int x, int y, int width, int height,
                                               int frame_width, int frame_height)
  : FilterPanel(start_frame, max_frame)
  , is_changed_(false)
{
  txt_x_.configure(create_adjustment(x, frame_width - 1), 10, 0);
  txt_y_.configure(create_adjustment(y, frame_height - 1), 10, 0);
  txt_width_.configure(create_adjustment(width, frame_width), 10, 0);
  txt_height_.configure(create_adjustment(height, frame_height), 10, 0);

  add_widget(txt_x_, _("_x:"), 0);
  add_widget(txt_y_, _("_y:"), 1);
  add_widget(txt_width_, _("_width:"), 2);
  add_widget(txt_height_, _("_height:"), 3);

  txt_x_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));
  txt_y_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));
  txt_width_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));
  txt_height_.signal_value_changed().connect(
    sigc::mem_fun(*this, &FilterPanelRectangular::on_coordinate_change));
}


Glib::RefPtr<Gtk::Adjustment> FilterPanelRectangular::create_adjustment(int start_value, int max)
{
  return Gtk::Adjustment::create(start_value, 0, max);
}


void FilterPanelRectangular::add_widget(Gtk::Widget& widget,
                                        const Glib::ustring& label, int row)
{
  Gtk::Label* l = Gtk::manage(new Gtk::Label(label, true));
  l->set_mnemonic_widget(widget);
  l->set_halign(Gtk::ALIGN_END);
  attach(*l, 0, row, 1, 1);
  attach_next_to(widget, *l, Gtk::POS_RIGHT, 1, 1);
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


FilterPanelDelogo::FilterPanelDelogo(int start_frame, int max_frame,
                                     int frame_width, int frame_height)
  : FilterPanelRectangular(start_frame, max_frame, frame_width, frame_height)
{
}


FilterPanelDelogo::FilterPanelDelogo(int start_frame, int max_frame,
                                     std::shared_ptr<fg::DelogoFilter> filter,
                                     int frame_width, int frame_height)
  : FilterPanelRectangular(start_frame, max_frame, filter, frame_width, frame_height)
{
}


fg::filter_ptr FilterPanelDelogo::get_filter() const
{
  return fg::filter_ptr(new fg::DelogoFilter(txt_x_.get_value_as_int(),
                                             txt_y_.get_value_as_int(),
                                             txt_width_.get_value_as_int(),
                                             txt_height_.get_value_as_int()));
}


FilterPanelDrawbox::FilterPanelDrawbox(int start_frame, int max_frame,
                                       int frame_width, int frame_height)
  : FilterPanelRectangular(start_frame, max_frame, frame_width, frame_height)
{
}


FilterPanelDrawbox::FilterPanelDrawbox(int start_frame, int max_frame,
                                       std::shared_ptr<fg::DrawboxFilter> filter,
                                       int frame_width, int frame_height)
  : FilterPanelRectangular(start_frame, max_frame, filter, frame_width, frame_height)
{
}


fg::filter_ptr FilterPanelDrawbox::get_filter() const
{
  return fg::filter_ptr(new fg::DrawboxFilter(txt_x_.get_value_as_int(),
                                              txt_y_.get_value_as_int(),
                                              txt_width_.get_value_as_int(),
                                              txt_height_.get_value_as_int()));
}
