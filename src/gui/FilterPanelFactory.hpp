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
#ifndef MDL_FILTER_PANEL_FACTORY_H
#define MDL_FILTER_PANEL_FACTORY_H

#include <gtkmm.h>

#include <boost/optional.hpp>

#include "filter-generator/Filters.hpp"

#include "common/Rectangle.hpp"


namespace mdl {
  class FilterPanel : public Gtk::Grid
  {
  protected:
    FilterPanel(int start_frame, int max_frame);
    virtual ~FilterPanel();

  public:
    typedef boost::optional<Rectangle> MaybeRectangle;

    virtual bool creates_filter() const;
    virtual fg::Filter* get_filter() const = 0;
    virtual void set_start_frame(int start_frame);
    virtual MaybeRectangle get_rectangle() const = 0;
    virtual void set_rectangle(const Rectangle& rect) = 0;
    virtual bool is_changed() const = 0;
    virtual void set_changed(bool changed) = 0;

    typedef sigc::signal<void, int> type_signal_start_frame_changed;
    virtual type_signal_start_frame_changed signal_start_frame_changed();

    typedef sigc::signal<void, Rectangle> type_signal_rectangle_changed;
    virtual type_signal_rectangle_changed signal_rectangle_changed();

  protected:
    Gtk::Label lbl_start_frame_;
    Gtk::SpinButton txt_start_frame_;

    type_signal_start_frame_changed signal_start_frame_changed_;
    type_signal_rectangle_changed signal_rectangle_changed_;

    void on_start_frame_change();
  };


  class FilterPanelFactory
  {
  public:
    FilterPanelFactory(int max_frame, int frame_width, int frame_height);

    FilterPanel* create(int start_frame, fg::Filter* filter);
    FilterPanel* create(int start_frame, fg::FilterType type);
    FilterPanel* convert(int start_frame, fg::Filter* original, fg::FilterType new_type);

  private:
    int max_frame_;
    int frame_width_;
    int frame_height_;

    bool is_no_parameters(fg::FilterType type);
    bool is_rectangular(fg::FilterType type);
  };
}

#endif // MDL_FILTER_PANEL_FACTORY_H
