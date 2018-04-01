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
#include <string>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/RegularScriptGenerator.hpp"
#include "filter-generator/FuzzyScriptGenerator.hpp"

#include "common/Exceptions.hpp"
#include "ETRProgressBar.hpp"
#include "EncodeWindow.hpp"
#include "FFmpegExecutor.hpp"
#include "MultiDelogoApp.hpp"
#include "Utils.hpp"

using namespace mdl;


EncodeWindow::EncodeWindow(std::unique_ptr<fg::FilterData> filter_data, int total_frames, double fps)
  : filter_data_(std::move(filter_data))
  , fps_(fps)
  , codec_(FFmpegExecutor::Codec::H264)
{
  set_title(_("Encode video"));
  set_border_width(8);

  Gtk::Grid* vbox = Gtk::manage(new Gtk::Grid());
  vbox->set_orientation(Gtk::ORIENTATION_VERTICAL);
  vbox->set_row_spacing(8);

  vbox->add(*create_file_selection());
  vbox->add(*create_codec());
  vbox->add(*create_quality());
  vbox->add(*create_fuzzy());
  vbox->add(*create_buttons());
  vbox->add(*create_progress());

  add(*vbox);

  ffmpeg_.set_total_frames(total_frames);
  ffmpeg_.signal_progress().connect(sigc::mem_fun(progress_bar_, &ETRProgressBar::set_progress));
  ffmpeg_.signal_finished().connect(sigc::mem_fun(*this, &EncodeWindow::on_ffmpeg_finished));
}


Gtk::Grid* EncodeWindow::create_file_selection()
{
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("_Output file:"), true));
  lbl->set_mnemonic_widget(txt_file_);

  Gtk::Button* btn = Gtk::manage(new Gtk::Button(_("_Select"), true));
  btn->set_image_from_icon_name("document-open");
  btn->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_select_file));

  Gtk::Grid* box = Gtk::manage(new Gtk::Grid());
  box->set_column_spacing(4);
  box->set_vexpand();
  box->set_valign(Gtk::ALIGN_CENTER);
  box->add(*lbl);
  txt_file_.set_hexpand();
  box->add(txt_file_);
  box->add(*btn);

  widgets_to_disable_.push_back(box);

  return box;
}


void EncodeWindow::on_select_file()
{
  Gtk::FileChooserDialog dlg(*this, _("Select output file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
  dlg.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
  dlg.add_button(_("_Save"), Gtk::RESPONSE_OK);
  dlg.set_current_folder(Glib::path_get_dirname(filter_data_->movie_file()));

  if (dlg.run() == Gtk::RESPONSE_OK) {
    txt_file_.set_text(dlg.get_file()->get_path());
  }
}


Gtk::Grid* EncodeWindow::create_codec()
{
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("Video format:")));

  Gtk::RadioButton* btn_h264 = Gtk::manage(new Gtk::RadioButton("H.26_4", true));
  btn_h264->set_tooltip_text(_("Most compatible video format. If in doubt, use this format"));
  btn_h264->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &EncodeWindow::on_codec),
               FFmpegExecutor::Codec::H264));

  Gtk::RadioButton* btn_h265 = Gtk::manage(new Gtk::RadioButton("H.26_5", true));
  btn_h265->join_group(*btn_h264);
  btn_h265->set_tooltip_text(_("A newer format that produces smaller video files. May not work on all players"));
  btn_h265->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &EncodeWindow::on_codec),
               FFmpegExecutor::Codec::H265));

  Gtk::Grid* box = Gtk::manage(new Gtk::Grid());
  box->set_column_spacing(4);
  box->set_vexpand();
  box->set_valign(Gtk::ALIGN_CENTER);
  box->add(*lbl);
  box->add(*btn_h264);
  box->add(*btn_h265);

  widgets_to_disable_.push_back(box);

  return box;
}


void EncodeWindow::on_codec(FFmpegExecutor::Codec codec)
{
  codec_ = codec;
  if (codec_ == FFmpegExecutor::Codec::H264) {
    txt_quality_.set_value(FFmpegExecutor::H264_DEFAULT_CRF_);
  } else {
    txt_quality_.set_value(FFmpegExecutor::H265_DEFAULT_CRF_);
  }
}


Gtk::Grid* EncodeWindow::create_quality()
{
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("_Quality:"), true));
  lbl->set_mnemonic_widget(txt_quality_);

  txt_quality_.set_range(0, 51);
  txt_quality_.set_increments(1, 1);
  txt_quality_.set_value(FFmpegExecutor::H264_DEFAULT_CRF_);
  txt_quality_.set_tooltip_text(_("CRF value to use for encoding. Lower values generally lead to higher quality, but bigger files. If in doubt, accept the default"));

  Gtk::Grid* box = Gtk::manage(new Gtk::Grid());
  box->set_column_spacing(4);
  box->set_vexpand();
  box->set_valign(Gtk::ALIGN_CENTER);
  box->add(*lbl);
  box->add(txt_quality_);

  widgets_to_disable_.push_back(box);

  return box;
}


Gtk::Grid* EncodeWindow::create_fuzzy()
{
  chk_fuzzy_.set_label(_("_Randomnly increase filter times"));
  chk_fuzzy_.set_use_underline();
  chk_fuzzy_.set_tooltip_text(_("If set, each filter's duration will be randomly increated, so that two or more filters can be active at the same time."));
  chk_fuzzy_.signal_toggled().connect(sigc::mem_fun(*this, &EncodeWindow::on_fuzzy_toggled));

  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("_Factor:"), true));
  lbl->set_mnemonic_widget(txt_fuzzyness_);

  txt_fuzzyness_.set_sensitive(false);
  txt_fuzzyness_.set_digits(1);
  txt_fuzzyness_.set_range(0.1, 10);
  txt_fuzzyness_.set_increments(0.1, 0.1);
  txt_fuzzyness_.set_value(2);
  txt_fuzzyness_.set_tooltip_text(_("Controls how much each filter's time is increased. If set to 2, then on average filters will last twice their original duration."));

  Gtk::Grid* box = Gtk::manage(new Gtk::Grid());
  box->set_column_spacing(4);
  box->set_vexpand();
  box->set_valign(Gtk::ALIGN_CENTER);
  box->add(chk_fuzzy_);
  lbl->set_margin_start(16);
  box->add(*lbl);
  box->add(txt_fuzzyness_);

  widgets_to_disable_.push_back(box);

  return box;
}


void EncodeWindow::on_fuzzy_toggled()
{
  txt_fuzzyness_.set_sensitive(chk_fuzzy_.get_active());
}


Gtk::Grid* EncodeWindow::create_buttons()
{
  Gtk::Button* btn_script = Gtk::manage(new Gtk::Button(_("_Generate filter script"), true));
  btn_script->set_tooltip_text(_("Generates a FFmpeg filter script file that can be used to encode the video. Use this option if you want to run FFmpeg manually with custom encoding options"));
  btn_script->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_generate_script));

  Gtk::Button* btn_encode = Gtk::manage(new Gtk::Button(_("_Encode"), true));
  btn_encode->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_encode));

  Gtk::Grid* box = Gtk::manage(new Gtk::Grid());
  box->set_column_spacing(8);
  box->set_vexpand();
  box->set_valign(Gtk::ALIGN_CENTER);
  box->set_halign(Gtk::ALIGN_END);
  box->add(*btn_script);
  box->add(*btn_encode);

  widgets_to_disable_.push_back(box);

  return box;
}


Gtk::Grid* EncodeWindow::create_progress()
{
  btn_log_.set_label(_("View _log"));
  btn_log_.set_use_underline();
  btn_log_.signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_view_log));

  box_progress_.set_orientation(Gtk::ORIENTATION_VERTICAL);
  box_progress_.set_row_spacing(4);
  box_progress_.set_vexpand();
  box_progress_.set_valign(Gtk::ALIGN_CENTER);
  box_progress_.set_no_show_all();

  lbl_status_.set_margin_top(16);
  lbl_status_.set_halign(Gtk::ALIGN_START);
  box_progress_.add(lbl_status_);
  progress_bar_.set_hexpand();
  box_progress_.add(progress_bar_);
  box_progress_.add(btn_log_);

  return &box_progress_;
}


void EncodeWindow::on_encode()
{
  std::string file = txt_file_.get_text();
  if (!check_file(file)) {
    return;
  }

  Generator generator = get_generator();
  ffmpeg_.set_generator(generator);
  ffmpeg_.set_input_file(filter_data_->movie_file());
  ffmpeg_.set_codec(codec_);
  ffmpeg_.set_quality(txt_quality_.get_value_as_int());
  ffmpeg_.set_output_file(file);

  try {
    ffmpeg_.encode();

    lbl_status_.set_text(_("Encoding in progress"));
    progress_bar_.reset();
    box_progress_.set_no_show_all(false);
    box_progress_.show_all();

    disable_widgets();
    btn_log_.hide();
  } catch (ScriptGenerationException& e) {
    Gtk::MessageDialog dlg(*this,
                           Glib::ustring::compose(_("Error generating filter script for FFmpeg: %1"), e.what()),
                           false, Gtk::MESSAGE_ERROR);
    dlg.run();
  } catch (FFmpegStartException& e) {
    auto msg = Glib::ustring::compose(_("Could not execute FFmpeg: %1"),
                                      e.what());
    Gtk::MessageDialog dlg(*this, msg, false, Gtk::MESSAGE_ERROR);
    dlg.run();
  }
}


void EncodeWindow::on_generate_script()
{
  std::string file = txt_file_.get_text();
  if (!check_file(file)) {
    return;
  }

  Generator generator = get_generator();
  ffmpeg_.set_generator(generator);

  try {
    ffmpeg_.generate_script(file);

    Gtk::MessageDialog dlg(*this, _("Filter script generated"));
    dlg.run();
  } catch (ScriptGenerationException& e) {
    auto msg = Glib::ustring::compose(_("Could not open file %1: %2"),
                                      file, e.what());
    Gtk::MessageDialog dlg(*this, msg, false, Gtk::MESSAGE_ERROR);
    dlg.run();
    return;
  }
}


bool EncodeWindow::check_file(const std::string& file)
{
  if (file.empty()) {
    Gtk::MessageDialog dlg(*this, _("Please select the output file"), false, Gtk::MESSAGE_ERROR);
    dlg.run();
    return false;
  }

  if (file_exists(file)) {
    return confirmation_dialog(*this,
                               Glib::ustring::compose(_("File %1 already exists. Overwrite?"), file),
                               _("_Overwrite"),  _("_Cancel"));
  } else {
    return true;
  }
}


EncodeWindow::Generator EncodeWindow::get_generator()
{
  Generator g;
  if (chk_fuzzy_.get_active()) {
    g = fg::FuzzyScriptGenerator::create(filter_data_->filter_list(), fps_, txt_fuzzyness_.get_value());
  }  else {
    g = fg::RegularScriptGenerator::create(filter_data_->filter_list(), fps_);
  }
  return g;
}


void EncodeWindow::on_ffmpeg_finished(bool success, const std::string& error)
{
  enable_widgets();
  btn_log_.show();

  progress_bar_.set_finished();

  if (success) {
    lbl_status_.set_text(_("Encoding finished successfully"));
  } else {
    lbl_status_.set_text(Glib::ustring::compose(_("Encoding failed: %1"), error));
  }
}


void EncodeWindow::disable_widgets()
{
  for (auto widget: widgets_to_disable_) {
    widget->set_sensitive(false);
  }
}


void EncodeWindow::enable_widgets()
{
  for (auto widget: widgets_to_disable_) {
    widget->set_sensitive(true);
  }
}


void EncodeWindow::on_view_log()
{
  LogWindow* window = new LogWindow(*this, ffmpeg_.get_log());
  get_application()->register_window(window);
}


bool EncodeWindow::on_delete_event(GdkEventAny*)
{
  if (!ffmpeg_.is_executing()) {
    return false;
  }

  bool terminate = confirmation_dialog(*this,
                                       _("Encoding is in progress. If it is cancelled now, it'll be necessary to restart encoding from the beginning. Really cancel?"),
                                       _("C_ancel encoding"), _("_Continue"));

  if (terminate) {
    ffmpeg_.terminate();
  }

  return !terminate;
}


LogWindow::LogWindow(Gtk::Window& parent, const std::string& log)
{
  set_title(_("FFmpeg log"));
  set_transient_for(parent);
  set_default_size(650, 300);

  Gtk::TextView* txt = Gtk::manage(new Gtk::TextView());
  txt->get_buffer()->set_text(log);

  Gtk::ScrolledWindow* scr = Gtk::manage(new Gtk::ScrolledWindow());
  scr->set_shadow_type(Gtk::SHADOW_IN);
  scr->add(*txt);

  add(*scr);
}
