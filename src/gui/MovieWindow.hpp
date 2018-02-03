#ifndef MOVIE_WINDOW_H
#define MOVIE_WINDOW_H

#include <gtkmm.h>

namespace mdl {
  class MovieWindow : public Gtk::ApplicationWindow
  {
  public:
    MovieWindow(const std::string& filename, int frame);

  private:
    Gtk::ScrolledWindow scroll_;
    Gtk::Image image_;
  };
}

#endif // MOVIE_WINDOW_H
