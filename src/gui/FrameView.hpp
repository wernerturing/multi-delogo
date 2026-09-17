/*
 * Copyright (C) 2018-2025 Werner Turing <werner.turing@protonmail.com>
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
    ~FrameView();

    void set_image(Glib::RefPtr<Gdk::Pixbuf> pixbuf);

    void set_zoom(gdouble level);

    void show_rectangle(const Rectangle& rect);
    void hide_rectangle();
    void scroll_to_current_rectangle();

    typedef sigc::signal<void(Rectangle)> type_signal_rectangle_changed;
    type_signal_rectangle_changed signal_rectangle_changed();

    typedef sigc::signal<void(int, int)> type_signal_size_changed;
    type_signal_size_changed signal_size_changed();

    static void on_canvas_motion_notify_wrapper(GtkEventControllerMotion* self,
                                                double x,
                                                double y,
                                                FrameView* frameview);
    static void on_canvas_leave_notify_wrapper(GtkEventControllerMotion *self,
                                               FrameView* frameview);

  private:
    const int image_width_;
    const int image_height_;

    Gtk::DrawingArea canvas_;
    Glib::RefPtr<Gtk::GestureMultiPress> gesture_click_;
    Glib::RefPtr<Gdk::Pixbuf> pixbuf_;
    SelectionRect* rect_;
    SelectionRect* temp_rect_;

    gdouble zoom_;

    bool drag_;
    Point drag_start_;

    type_signal_rectangle_changed signal_rectangle_changed_;
    type_signal_size_changed signal_size_changed_;


    void update_canvas_size();
    // Offset, in widget pixels, of the (possibly centered) scaled image
    // inside the drawing area allocation.
    Point content_offset() const;
    // Maps a pointer position on the drawing area to image-space pixels.
    Point widget_to_image(double x, double y) const;

    bool render_canvas(const Cairo::RefPtr<Cairo::Context>& cr);
    void draw_selection(const Cairo::RefPtr<Cairo::Context>& cr, SelectionRect& rect);

    void on_size_allocate(Gtk::Allocation& allocation) override;

    void on_canvas_button_press(int n_press, double x, double y);
    void on_canvas_button_release(int n_press, double x, double y);
    void on_canvas_motion_notify(double x, double y);
    void on_canvas_leave_notify();
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


  class SelectionRect
  {
  public:
    SelectionRect(gdouble x=0.0, gdouble y=0.0, gdouble width=0.0, gdouble height=0.0);

    void set_visible(bool is_visible);
    bool is_visible() const;

    void create_cursors();

    Rectangle get_coordinates() const;
    void set_coordinates(const Rectangle& coordinates);

    typedef sigc::signal<void(Rectangle)> type_signal_rectangle_changed;
    type_signal_rectangle_changed signal_rectangle_changed();

    // Hit-testing and interactive move/resize. Every point is in
    // image-space coordinates; the caller is responsible for translating
    // widget/event coordinates and for filtering the mouse button.
    bool contains(const Point& point) const;
    Glib::RefPtr<Gdk::Cursor> cursor_for_point(const Point& point) const;

    void begin_drag(const Point& point);
    void update_drag(const Point& point);
    void end_drag();
    bool dragging() const;


  private:
    const static gdouble RESIZE_MARGIN_;

    Rectangle coordinates_;
    bool visible_;

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


    Rectangle normalize(const Rectangle& original) const;

    Point to_inside_coordinates(const Point& point) const;
    DragMode get_drag_mode_for_point(const Point& point) const;
    Glib::RefPtr<Gdk::Cursor> get_cursor(DragMode mode) const;

    void start_drag(DragMode mode, Point start);
    Rectangle get_new_coordinates(const Point& drag_point) const;


    friend class SelectionRectTestFixture;
  };
}

#endif // MDL_FRAME_VIEW_H
