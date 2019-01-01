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

#include "FilterType.hpp"

using namespace mdl;


FilterType::FilterType(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
  : Gtk::Grid(cobject)
  , rad_delogo_(nullptr)
  , rad_drawbox_(nullptr)
  , rad_cut_(nullptr)
  , rad_none_(nullptr)
  , rad_review_(nullptr)
{
  builder->get_widget("rad_delogo", rad_delogo_);
  builder->get_widget("rad_drawbox", rad_drawbox_);
  builder->get_widget("rad_cut", rad_cut_);
  builder->get_widget("rad_none", rad_none_);
  builder->get_widget("rad_review", rad_review_);

  rad_delogo_->signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        *rad_delogo_));
  rad_drawbox_->signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        *rad_drawbox_));
  rad_cut_->signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        *rad_cut_));
  rad_none_->signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        *rad_none_));
  rad_review_->signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        *rad_review_));
}


void FilterType::set(fg::FilterType type)
{
  switch (type) {
  case fg::FilterType::DELOGO:
    rad_delogo_->set_active();
    break;

  case fg::FilterType::DRAWBOX:
    rad_drawbox_->set_active();
    break;

  case fg::FilterType::CUT:
    rad_cut_->set_active();
    break;

  case fg::FilterType::REVIEW:
    rad_review_->set_active();
    break;

  case fg::FilterType::NO_OP:
  default: // Avoid compiler warning
    rad_none_->set_active();
    break;
  }
}


fg::FilterType FilterType::get() const
{
  if (rad_delogo_->get_active()) {
    return fg::FilterType::DELOGO;
  } else if (rad_drawbox_->get_active()) {
    return fg::FilterType::DRAWBOX;
  } else if (rad_cut_->get_active()) {
    return fg::FilterType::CUT;
  } else if (rad_review_->get_active()) {
    return fg::FilterType::REVIEW;
 } else {
    return fg::FilterType::NO_OP;
  }
}


FilterType::type_signal_type_changed FilterType::signal_type_changed()
{
  return signal_type_changed_;
}


void FilterType::on_radio_toggled(const Gtk::RadioButton& radio)
{
  if (radio.get_active()) {
    signal_type_changed_.emit(get());
  }
}
