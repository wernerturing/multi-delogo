/*
 * Copyright (C) 2018 Werner Turing <werner.turing@protonmail.com>
 *
 * This file is part of multi-delogo.
 *
 * multi-delogo is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * multi-delogo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with multi-delogo.  If not, see <http://www.gnu.org/licenses/>.
 */
#include <memory>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "EncodeWindow.hpp"

using namespace mdl;


EncodeWindow::EncodeWindow(std::unique_ptr<fg::FilterData> filter_data)
  : filter_data_(std::move(filter_data))
  , codec_(Codec::H264)
{
  set_title(_("Encode video"));
  set_border_width(8);

  Gtk::Box* vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 8));

  vbox->pack_start(*create_file_selection(), true, true);
  vbox->pack_start(*create_codec(), true, true);
  vbox->pack_start(*create_quality(), true, true);

  add(*vbox);
}


Gtk::Box* EncodeWindow::create_file_selection()
{
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("_Output file:"), true));
  lbl->set_mnemonic_widget(txt_file_);

  Gtk::Button* btn = Gtk::manage(new Gtk::Button(_("_Select"), true));
  btn->set_image_from_icon_name("document-open");
  btn->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_select_file));

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
  box->pack_start(*lbl, false, false);
  box->pack_start(txt_file_, true, true);
  box->pack_start(*btn, false, false);

  return box;
}


void EncodeWindow::on_select_file()
{
  Gtk::FileChooserDialog dlg(*this, _("Select output file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
  dlg.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
  dlg.add_button(_("_Save"), Gtk::RESPONSE_OK);

  if (dlg.run() == Gtk::RESPONSE_OK) {
    txt_file_.set_text(dlg.get_file()->get_path());
  }
}


Gtk::Box* EncodeWindow::create_codec()
{
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("Video format:")));

  Gtk::RadioButton* btn_h264 = Gtk::manage(new Gtk::RadioButton("H.26_4", true));
  btn_h264->set_tooltip_text(_("Most compatible video format. If in doubt, use this format"));
  btn_h264->signal_toggled().connect(
    sigc::bind<Codec>(
      sigc::mem_fun(*this, &EncodeWindow::on_codec),
      Codec::H264));

  Gtk::RadioButton* btn_h265 = Gtk::manage(new Gtk::RadioButton("H.26_5", true));
  btn_h265->join_group(*btn_h264);
  btn_h265->set_tooltip_text(_("A newer format that produces smaller video files. May not work on all players"));
  btn_h265->signal_toggled().connect(
    sigc::bind<Codec>(
      sigc::mem_fun(*this, &EncodeWindow::on_codec),
      Codec::H265));

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
  box->pack_start(*lbl, false, false);
  box->pack_start(*btn_h264, false, false);
  box->pack_start(*btn_h265, false, false);

  return box;
}


void EncodeWindow::on_codec(Codec codec)
{
  codec_ = codec;
  if (codec_ == Codec::H264) {
    txt_quality_.set_value(H264_DEFAULT_CRF_);
  } else {
    txt_quality_.set_value(H265_DEFAULT_CRF_);
  }
}


Gtk::Box* EncodeWindow::create_quality()
{
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("_Quality:"), true));
  lbl->set_mnemonic_widget(txt_quality_);

  txt_quality_.set_range(0, 51);
  txt_quality_.set_increments(1, 1);
  txt_quality_.set_value(H264_DEFAULT_CRF_);
  txt_quality_.set_tooltip_text(_("CRF value to use for encoding. Lower values generally lead to higher quality, but bigger files. If in doubt, accept the default"));

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
  box->pack_start(*lbl, false, false);
  box->pack_start(txt_quality_, false, true);

  return box;
}
