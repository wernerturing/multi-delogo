#include <gtkmm.h>

#include "MovieWindow.hpp"


int main(int argc, char *argv[])
{
  Gtk::Main gtk(argc, argv);

  if (argc != 3) {
    printf("usage: gtkshowframe <Video_Path> <Frame_Number>\n");
    return -1;
  }

  int frameNumber = atoi(argv[2]);

  mdl::MovieWindow window(argv[1], frameNumber);

  window.show_all();
  gtk.run(window);
}
