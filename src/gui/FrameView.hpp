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
#ifndef MDL_FRAME_VIEW_H
#define MDL_FRAME_VIEW_H

#include <gtkmm.h>
#include <goocanvasmm.h>

#include "common/Rectangle.hpp"


namespace mdl {
  class SelectionRect;


  class FrameView : public Gtk::ScrolledWindow
  {
  public:
    FrameView(BaseObjectType* cobject,
              const Glib::RefPtr<Gtk::Builder>& builder,
              int width, int height,
              bool can_select_rectangle = true);

    void set_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf);

    void set_zoom(gdouble level);

    void show_rectangle(const Rectangle& rect);
    void hide_rectangle();
    void scroll_to_current_rectangle();

    typedef sigc::signal<void, Rectangle> type_signal_rectangle_changed;
    type_signal_rectangle_changed signal_rectangle_changed();

  private:
    Goocanvas::Canvas canvas_;
    Glib::RefPtr<Goocanvas::Image> image_;
    Glib::RefPtr<SelectionRect> rect_;
    Glib::RefPtr<SelectionRect> temp_rect_;

    bool drag_;
    Point drag_start_;

    type_signal_rectangle_changed signal_rectangle_changed_;


    bool on_button_press(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event);
    bool on_motion_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventMotion* event);
    bool on_button_release(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event);
  };


  enum class DragMode
  {
    NONE,
    MOVE,
    RESIZE_BR,
    RESIZE_BL,
    RESIZE_TL,
    RESIZE_TR,
    RESIZE_B,
    RESIZE_L,
    RESIZE_T,
    RESIZE_R,
  };


  class SelectionRect : public Goocanvas::Rect
  {
  protected:
    SelectionRect(gdouble x, gdouble y, gdouble width, gdouble height);

  public:
    static Glib::RefPtr<SelectionRect> create(gdouble x=0.0, gdouble y=0.0, gdouble width=0.0, gdouble height=0.0);
    void enable_drag_and_drop();

    Rectangle get_coordinates();
    void set_coordinates(const Rectangle& coordinates);

    typedef sigc::signal<void, Rectangle> type_signal_rectangle_changed;
    type_signal_rectangle_changed signal_rectangle_changed();


  private:
    const static gdouble RESIZE_MARGIN_;

    DragMode drag_mode_;
    Rectangle start_coordinates_;
    Point drag_start_;

    Glib::RefPtr<Gdk::Cursor> move_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_br_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_bl_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_tl_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_tr_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_b_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_l_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_t_cursor_;
    Glib::RefPtr<Gdk::Cursor> resize_r_cursor_;

    type_signal_rectangle_changed signal_rectangle_changed_;


    Rectangle normalize(const Rectangle& original);

    Point to_inside_coordinates(const Point& point);
    DragMode get_drag_mode_for_point(const Point& point);
    Glib::RefPtr<Gdk::Cursor> get_cursor(DragMode mode);

    void start_drag(DragMode mode, Point start);
    Rectangle get_new_coordinates(const Point& drag_point);

    bool on_button_press(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event);
    bool on_motion_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventMotion* event);
    bool on_button_release(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventButton* event);

    bool on_leave_notify(const Glib::RefPtr<Goocanvas::Item>& item, GdkEventCrossing* event);


    friend class SelectionRectTestFixture;
  };
}

#endif // MDL_FRAME_VIEW_H
