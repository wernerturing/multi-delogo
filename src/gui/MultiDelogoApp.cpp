#include <gtkmm.h>

#include "common/Exceptions.hpp"
#include "common/FrameProvider.hpp"

#include "MultiDelogoApp.hpp"
#include "MovieWindow.hpp"

using namespace mdl;


MultiDelogoApp::MultiDelogoApp()
  : Gtk::Application("wt.multi-delogo", Gio::APPLICATION_HANDLES_OPEN) {
}


Glib::RefPtr<MultiDelogoApp> MultiDelogoApp::create() {
  return Glib::RefPtr<MultiDelogoApp>(new MultiDelogoApp());
}


void MultiDelogoApp::on_activate() {
  printf("For now, provide a movie file as argument.\n");
}


void MultiDelogoApp::on_open(const Gio::Application::type_vec_files& files,
                             const Glib::ustring& hint) {
  for (Glib::RefPtr<Gio::File> file: files) {
    create_movie_window(file);
  }
}


void MultiDelogoApp::create_movie_window(const Glib::RefPtr<Gio::File>& file)
{
  try {
    auto frame_provider = create_frame_provider(file->get_path());
    MovieWindow* window = new MovieWindow(frame_provider);
    register_window(window);
  } catch (const VideoNotOpenedException& e) {
    auto msg = Glib::ustring::compose("Could not open file %1", file->get_path());
    Gtk::MessageDialog dlg(msg, false, Gtk::MESSAGE_ERROR);
    dlg.run();
  }
}


void MultiDelogoApp::register_window(Gtk::ApplicationWindow* window) {
  window->signal_hide().connect(
    sigc::bind<Gtk::ApplicationWindow*>(
      sigc::mem_fun(*this, &MultiDelogoApp::on_hide_window),
      window));

  add_window(*window);
  window->show_all();
  window->present();
}


void MultiDelogoApp::on_hide_window(Gtk::ApplicationWindow* window) {
  delete window;
}
