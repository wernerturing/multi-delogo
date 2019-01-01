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
#ifndef MDL_FRAME_NAVIGATOR_H
#define MDL_FRAME_NAVIGATOR_H

#include <gtkmm.h>

#include "common/FrameProvider.hpp"

#include "NumericEntry.hpp"
#include "FrameView.hpp"


namespace mdl {
  class FrameNavigator : public Gtk::Grid
  {
  public:
    FrameNavigator(BaseObjectType* cobject,
                   const Glib::RefPtr<Gtk::Builder>& builder,
                   Gtk::Window& parent_window,
                   const Glib::RefPtr<FrameProvider>& frame_provider);

    int get_number_of_frames() const;
    int get_frame_width() const;
    int get_frame_height() const;
    double get_fps() const;
    void single_step_frame(int direction);
    void jump_step_frame(int direction);
    void change_displayed_frame(int new_frame_number);

    int get_jump_size() const;
    void set_jump_size(int jump_size);

    enum class PrevFrame { NO, FIT, SAME };
    void set_show_prev_frame(PrevFrame setting);

    FrameView* get_frame_view();

    typedef sigc::signal<void, int> type_signal_frame_changed;
    type_signal_frame_changed signal_frame_changed();

  private:
    Gtk::Window& parent_window_;

    Glib::RefPtr<FrameProvider> frame_provider_;
    int number_of_frames_;
    int frame_number_;

    Glib::RefPtr<Gdk::Pixbuf> frame_pixbuf_;
    FrameView* frame_view_;
    Glib::RefPtr<Gdk::Pixbuf> prev_frame_pixbuf_;
    FrameView* prev_frame_view_;
    Glib::RefPtr<Gdk::Pixbuf> empty_pixbuf_;
    Gtk::Label* lbl_prev_frame_;

    NumericEntry* txt_frame_number_;
    NumericEntry* txt_jump_size_;

    gdouble zoom_;
    Gtk::Label* lbl_zoom_;
    Gtk::Button* btn_zoom_out_;
    Gtk::Button* btn_zoom_in_;
    Gtk::Button* btn_zoom_100_;

    type_signal_frame_changed signal_frame_changed_;

    PrevFrame prev_frame_setting_;
    sigc::connection prev_frame_view_on_size_allocate_;


    void configure_navigation_bar(const Glib::RefPtr<Gtk::Builder>& builder);
    void configure_zoom_bar(const Glib::RefPtr<Gtk::Builder>& builder);

    void show_next_frame(int new_frame_number);
    void show_previous_frame(int new_frame_number);
    void show_frame(int new_frame_number);
    void fetch_and_show_current_frame(int new_frame_number);
    void fetch_and_show_prev_frame(int new_frame_number);

    void on_frame_number_activate();
    bool on_frame_number_input(GdkEventFocus*);

    void on_step_zoom(gdouble increment);
    void on_zoom_100();
    void on_zoom_fit();
    void set_zoom(gdouble zoom);
    void set_prev_frame_zoom(Gtk::Allocation size);
  };
}

#endif // MDL_FRAME_NAVIGATOR_H
