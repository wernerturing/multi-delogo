#ifndef MDL_MULTI_DELOGO_APP_H
#define MDL_MULTI_DELOGO_APP_H

#include <gtkmm.h>


namespace mdl {
  class MultiDelogoApp : public Gtk::Application
  {
  protected:
    MultiDelogoApp();

  public:
    static Glib::RefPtr<MultiDelogoApp> create();

  protected:
    void on_activate();
    void on_open(const Gio::Application::type_vec_files& files,
                 const Glib::ustring& hint);

  private:
    void create_movie_window(const Glib::RefPtr<Gio::File>& file);
    void register_window(Gtk::ApplicationWindow* window);

    void on_hide_window(Gtk::ApplicationWindow* window);
  };
}

#endif // MDL_MULTI_DELOGO_APP_H
