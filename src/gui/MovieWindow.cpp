#include <string>

#include <gtkmm.h>

#include "common/Exceptions.hpp"
#include "common/FrameProvider.hpp"

#include "MovieWindow.hpp"

using namespace mdl;


MovieWindow::MovieWindow(const Glib::RefPtr<FrameProvider>& frame_provider)
  : frame_provider_(frame_provider)
{
  set_default_size(600, 600);

  Gtk::Box* vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 8));
  vbox->pack_start(scroll_, true, true);

  scroll_.add(image_);

  vbox->pack_start(*create_navigation_box(), false, false);

  add(*vbox);

  change_displayed_frame(1000);
}


Gtk::Box* MovieWindow::create_navigation_box()
{
  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));

  txt_frame_number_.set_width_chars(6);
  txt_frame_number_.set_text("1000");

  Gtk::Button* btn_prev = Gtk::manage(new Gtk::Button("<"));
  btn_prev->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_single_step_frame),
      -1));

  Gtk::Button* btn_next = Gtk::manage(new Gtk::Button(">"));
  btn_next->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_single_step_frame),
      1));

  txt_frame_number_.signal_activate().connect(
    sigc::mem_fun(*this, &MovieWindow::on_frame_number_activate));
  txt_frame_number_.signal_focus_out_event().connect(
    sigc::mem_fun(*this, &MovieWindow::on_frame_number_input));

  box->pack_start(*btn_prev, false, false);
  box->pack_start(txt_frame_number_, false, false);
  box->pack_start(*btn_next, false, false);

  return box;
}


void MovieWindow::change_displayed_frame(int new_frame_number)
{
  try {
    auto pixbuf = frame_provider_->get_frame(new_frame_number);
    image_.set(pixbuf);

    frame_number_ = new_frame_number;
    txt_frame_number_.set_text(std::to_string(frame_number_));
  } catch (const FrameNotAvailableException& e) {
    Gtk::MessageDialog dlg(*this,
                           "Could not get frame", false,
                           Gtk::MESSAGE_ERROR);
    txt_frame_number_.set_text(std::to_string(frame_number_));
    dlg.run();
  }
}


void MovieWindow::on_single_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + direction);
}


void MovieWindow::on_frame_number_activate()
{
  change_displayed_frame(std::stoi(txt_frame_number_.get_text()));
}


bool MovieWindow::on_frame_number_input(GdkEventFocus*)
{
  change_displayed_frame(std::stoi(txt_frame_number_.get_text()));
  return false;
}
