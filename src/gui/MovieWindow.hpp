#ifndef MOVIE_WINDOW_H
#define MOVIE_WINDOW_H

#include <gtkmm.h>

#include "common/FrameProvider.hpp"

#include "NumericEntry.hpp"


namespace mdl {
  class MovieWindow : public Gtk::ApplicationWindow
  {
  public:
    MovieWindow(const Glib::RefPtr<FrameProvider>& frame_provider);

  private:
    Glib::RefPtr<FrameProvider> frame_provider_;
    int frame_number_;

    Gtk::ScrolledWindow scroll_;
    Gtk::Image image_;

    NumericEntry txt_frame_number_;
    NumericEntry txt_jump_size_;


    Gtk::Box* create_navigation_box();

    void change_displayed_frame(int new_frame_number);

    void on_single_step_frame(int direction);
    void on_jump_step_frame(int direction);
    void on_frame_number_activate();
    bool on_frame_number_input(GdkEventFocus*);
  };
}

#endif // MOVIE_WINDOW_H
