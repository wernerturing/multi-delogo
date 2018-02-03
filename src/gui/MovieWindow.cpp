#include <gtkmm.h>

#include "MovieWindow.hpp"
#include "common/FrameProvider.hpp"

using namespace mdl;


MovieWindow::MovieWindow(const std::string& filename, int frame_number)
  : Gtk::ApplicationWindow(), scroll_(), image_()
{
  frame_provider_ = create_frame_provider(filename);
  auto pixbuf = frame_provider_->get_frame(frame_number);
  image_.set(pixbuf);

  scroll_.add(image_);

  set_default_size(600, 600);
  add(scroll_);
}
