#include <gtkmm.h>
#include <glibmm/i18n.h>

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
  txt_jump_size_.set_value(500);

  signal_key_press_event().connect(sigc::mem_fun(*this, &MovieWindow::on_key_press));
}


Gtk::Box* MovieWindow::create_navigation_box()
{
  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));

  txt_frame_number_.set_width_chars(6);

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

  Gtk::Button* btn_prev_jump = Gtk::manage(new Gtk::Button("<<"));
  btn_prev_jump->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_jump_step_frame),
      -1));

  Gtk::Button* btn_next_jump = Gtk::manage(new Gtk::Button(">>"));
  btn_next_jump->signal_clicked().connect(
    sigc::bind<int>(
      sigc::mem_fun(*this, &MovieWindow::on_jump_step_frame),
      1));

  txt_frame_number_.signal_activate().connect(
    sigc::mem_fun(*this, &MovieWindow::on_frame_number_activate));
  txt_frame_number_.signal_focus_out_event().connect(
    sigc::mem_fun(*this, &MovieWindow::on_frame_number_input));

  box->pack_start(*btn_prev_jump, false, false);
  box->pack_start(*btn_prev, false, false);
  box->pack_start(txt_frame_number_, false, false);
  box->pack_start(*btn_next, false, false);
  box->pack_start(*btn_next_jump, false, false);

  box->pack_start(*Gtk::manage(new Gtk::Label()), false, false, 16);

  Gtk::Label* lbl_jump_size = Gtk::manage(new Gtk::Label(_("_Jump size:"), true));
  lbl_jump_size->set_mnemonic_widget(txt_jump_size_);
  box->pack_start(*lbl_jump_size, false, false);

  txt_jump_size_.set_width_chars(6);
  box->pack_start(txt_jump_size_, false, false);

  return box;
}


void MovieWindow::change_displayed_frame(int new_frame_number)
{
  try {
    auto pixbuf = frame_provider_->get_frame(new_frame_number);
    image_.set(pixbuf);

    frame_number_ = new_frame_number;
    txt_frame_number_.set_value(frame_number_);
  } catch (const FrameNotAvailableException& e) {
    Gtk::MessageDialog dlg(*this,
                           _("Could not get frame"), false,
                           Gtk::MESSAGE_ERROR);
    txt_frame_number_.set_value(frame_number_);
    dlg.run();
  }
}


void MovieWindow::on_single_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + direction);
}


void MovieWindow::on_jump_step_frame(int direction)
{
  change_displayed_frame(frame_number_ + txt_jump_size_.get_value()*direction);
}


void MovieWindow::on_frame_number_activate()
{
  change_displayed_frame(txt_frame_number_.get_value());
}


bool MovieWindow::on_frame_number_input(GdkEventFocus*)
{
  change_displayed_frame(txt_frame_number_.get_value());
  return false;
}


bool MovieWindow::on_key_press(GdkEventKey* key_event)
{
  switch (key_event->keyval) {
  case GDK_KEY_A:
  case GDK_KEY_a:
    on_jump_step_frame(-1);
    return true;

  case GDK_KEY_S:
  case GDK_KEY_s:
    on_single_step_frame(-1);
    return true;

  case GDK_KEY_D:
  case GDK_KEY_d:
    on_single_step_frame(1);
    return true;

  case GDK_KEY_F:
  case GDK_KEY_f:
    on_jump_step_frame(1);
    return true;
  }

  return false;
}
