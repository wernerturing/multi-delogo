#include <gtkmm.h>


int main(int argc, char *argv[])
{
  Gtk::Main gtk(argc, argv);

  if (argc != 2) {
    printf("usage: gtkshowframe <Video_Path> <Frame_Number>\n");
    return -1;
  }

  Gtk::Window window;
  window.set_default_size(600, 600);

  auto pixbuf = Gdk::Pixbuf::create_from_file(argv[1]);
  Gtk::Image image(pixbuf);
  window.add(image);

  window.show_all();
  gtk.run(window);
}
