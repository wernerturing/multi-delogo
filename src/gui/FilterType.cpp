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

#include "FilterType.hpp"

using namespace mdl;


FilterType::FilterType()
  : Gtk::Grid()
  , rad_delogo_("delogo")
  , rad_drawbox_("drawbox")
  , rad_cut_("cut")
  , rad_none_("none")
  , rad_review_("review")
{
  rad_drawbox_.join_group(rad_delogo_);
  rad_cut_.join_group(rad_delogo_);
  rad_none_.join_group(rad_delogo_);
  rad_review_.join_group(rad_delogo_);

  rad_delogo_.set_halign(Gtk::ALIGN_START);
  rad_drawbox_.set_halign(Gtk::ALIGN_START);
  rad_cut_.set_halign(Gtk::ALIGN_START);
  rad_none_.set_halign(Gtk::ALIGN_START);
  rad_review_.set_halign(Gtk::ALIGN_START);

  rad_delogo_.signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        rad_delogo_));
  rad_drawbox_.signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        rad_drawbox_));
  rad_cut_.signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        rad_cut_));
  rad_none_.signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        rad_none_));
  rad_review_.signal_toggled().connect(
    sigc::bind<const Gtk::RadioButton&>(sigc::mem_fun(*this, &FilterType::on_radio_toggled),
                                        rad_review_));

  set_column_spacing(4);

  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("Filter:")));
  attach(*lbl, 0, 0, 1, 1);

  attach_next_to(rad_delogo_, *lbl, Gtk::POS_RIGHT, 1, 1);
  attach_next_to(rad_drawbox_, rad_delogo_, Gtk::POS_BOTTOM, 1, 1);
  attach_next_to(rad_cut_, rad_drawbox_, Gtk::POS_BOTTOM, 1, 1);
  attach_next_to(rad_none_, rad_cut_, Gtk::POS_BOTTOM, 1, 1);
  attach_next_to(rad_review_, rad_none_, Gtk::POS_BOTTOM, 1, 1);
}


void FilterType::set(fg::FilterType type)
{
  switch (type) {
  case fg::FilterType::DELOGO:
    rad_delogo_.set_active();
    break;

  case fg::FilterType::DRAWBOX:
    rad_drawbox_.set_active();
    break;

  case fg::FilterType::CUT:
    rad_cut_.set_active();
    break;

  case fg::FilterType::REVIEW:
    rad_review_.set_active();
    break;

  case fg::FilterType::NO_OP:
  default: // Avoid compiler warning
    rad_none_.set_active();
    break;
  }
}


fg::FilterType FilterType::get() const
{
  if (rad_delogo_.get_active()) {
    return fg::FilterType::DELOGO;
  } else if (rad_drawbox_.get_active()) {
    return fg::FilterType::DRAWBOX;
  } else if (rad_cut_.get_active()) {
    return fg::FilterType::CUT;
  } else if (rad_review_.get_active()) {
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
