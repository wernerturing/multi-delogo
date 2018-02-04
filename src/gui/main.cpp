#include <gtkmm.h>

#include "common/Exceptions.hpp"
#include "common/FrameProvider.hpp"

#include "MovieWindow.hpp"


int main(int argc, char *argv[])
{
  Gtk::Main gtk(argc, argv);

  if (argc != 2) {
    printf("usage: multi-delogo <Video_Path>\n");
    return -1;
  }

  try {
    auto frame_provider = mdl::create_frame_provider(argv[1]);

    mdl::MovieWindow window(frame_provider);

    window.show_all();
    gtk.run(window);
  } catch (const mdl::VideoNotOpenedException& e) {
    printf("Could not open file %s\n", argv[1]);
    return 1;
  }
}
