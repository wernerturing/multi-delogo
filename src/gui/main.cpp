#include <gtkmm.h>

#include "MovieWindow.hpp"


int main(int argc, char *argv[])
{
  Gtk::Main gtk(argc, argv);

  if (argc != 2) {
    printf("usage: multi-delogo <Video_Path>\n");
    return -1;
  }

  mdl::MovieWindow window(argv[1]);

  window.show_all();
  gtk.run(window);
}
