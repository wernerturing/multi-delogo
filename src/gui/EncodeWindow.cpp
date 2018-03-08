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
#include <iomanip>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/RegularScriptGenerator.hpp"
#include "filter-generator/FuzzyScriptGenerator.hpp"

#include "common/Exceptions.hpp"
#include "EncodeWindow.hpp"
#include "FFmpegExecutor.hpp"
#include "Utils.hpp"

using namespace mdl;


EncodeWindow::EncodeWindow(std::unique_ptr<fg::FilterData> filter_data, int total_frames, double fps)
  : filter_data_(std::move(filter_data))
  , fps_(fps)
  , codec_(FFmpegExecutor::Codec::H264)
{
  set_title(_("Encode video"));
  set_border_width(8);

  Gtk::Box* vbox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL, 8));

  vbox->pack_start(*create_file_selection(), true, true);
  vbox->pack_start(*create_codec(), true, true);
  vbox->pack_start(*create_quality(), true, true);
  vbox->pack_start(*create_fuzzy(), true, true);
  vbox->pack_start(*create_buttons(), true, true);
  vbox->pack_start(*create_progress(), true, true);

  add(*vbox);

  ffmpeg_.set_total_frames(total_frames);
  ffmpeg_.signal_progress().connect(sigc::mem_fun(*this, &EncodeWindow::on_ffmpeg_progress));
  ffmpeg_.signal_finished().connect(sigc::mem_fun(*this, &EncodeWindow::on_ffmpeg_finished));
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


Gtk::Box* EncodeWindow::create_codec()
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

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
  box->pack_start(*lbl, false, false);
  box->pack_start(*btn_h264, false, false);
  box->pack_start(*btn_h265, false, false);

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


Gtk::Box* EncodeWindow::create_quality()
{
  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("_Quality:"), true));
  lbl->set_mnemonic_widget(txt_quality_);

  txt_quality_.set_range(0, 51);
  txt_quality_.set_increments(1, 1);
  txt_quality_.set_value(FFmpegExecutor::H264_DEFAULT_CRF_);
  txt_quality_.set_tooltip_text(_("CRF value to use for encoding. Lower values generally lead to higher quality, but bigger files. If in doubt, accept the default"));

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
  box->pack_start(*lbl, false, false);
  box->pack_start(txt_quality_, false, true);

  widgets_to_disable_.push_back(box);

  return box;
}


Gtk::Box* EncodeWindow::create_fuzzy()
{
  chk_fuzzy_.set_label(_("_Randomnly increase filter times"));
  chk_fuzzy_.set_use_underline();
  chk_fuzzy_.set_tooltip_text(_("If set, each filter's duration will be randomly increated, so that two or more filters can be active at the same time."));
  chk_fuzzy_.signal_toggled().connect(sigc::mem_fun(*this, &EncodeWindow::on_fuzzy_toggled));

  Gtk::Label* lbl = Gtk::manage(new Gtk::Label(_("_Factor:"), true));
  lbl->set_mnemonic_widget(txt_fuzzyness_);
  lbl->set_margin_start(16);

  txt_fuzzyness_.set_sensitive(false);
  txt_fuzzyness_.set_digits(1);
  txt_fuzzyness_.set_range(0.1, 10);
  txt_fuzzyness_.set_increments(0.1, 0.1);
  txt_fuzzyness_.set_value(2);
  txt_fuzzyness_.set_tooltip_text(_("Controls how much each filter's time is increased. If set to 2, then on average filters will last twice their original duration."));

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
  box->pack_start(chk_fuzzy_, false, false);
  box->pack_start(*lbl, false, false);
  box->pack_start(txt_fuzzyness_, false, false);

  widgets_to_disable_.push_back(box);

  return box;
}


void EncodeWindow::on_fuzzy_toggled()
{
  txt_fuzzyness_.set_sensitive(chk_fuzzy_.get_active());
}


Gtk::Box* EncodeWindow::create_buttons()
{
  Gtk::Button* btn_script = Gtk::manage(new Gtk::Button(_("_Generate filter script"), true));
  btn_script->set_tooltip_text(_("Generates a FFmpeg filter script file that can be used to encode the video. Use this option if you want to run FFmpeg manually with custom encoding options"));
  btn_script->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_generate_script));

  Gtk::Button* btn_encode = Gtk::manage(new Gtk::Button(_("_Encode"), true));
  btn_encode->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_encode));

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 8));
  box->pack_end(*btn_encode, false, false);
  box->pack_end(*btn_script, false, false);

  widgets_to_disable_.push_back(box);

  return box;
}


Gtk::Box* EncodeWindow::create_progress()
{
  lbl_status_.set_margin_top(16);
  lbl_status_.set_halign(Gtk::ALIGN_START);

  progress_bar_.set_show_text();

  box_progress_.set_orientation(Gtk::ORIENTATION_VERTICAL);
  box_progress_.set_spacing(4);
  box_progress_.set_no_show_all();

  box_progress_.pack_start(lbl_status_, true, true);
  box_progress_.pack_start(progress_bar_, true, true);

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
    progress_bar_.set_fraction(0);
    box_progress_.set_no_show_all(false);
    box_progress_.show_all();

    disable_widgets();
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
    Gtk::MessageDialog dlg(*this,
                           Glib::ustring::compose(_("File %1 already exists. Overwrite?"), file),
                           false,
                           Gtk::MESSAGE_QUESTION,
                           Gtk::BUTTONS_NONE);
    dlg.add_button(_("_Cancel"), Gtk::RESPONSE_NO);
    dlg.add_button(_("_Overwrite"), Gtk::RESPONSE_YES);
    return dlg.run() == Gtk::RESPONSE_YES;
  }

  return true;
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


void EncodeWindow::on_ffmpeg_progress(const FFmpegExecutor::Progress& p)
{
  if (p.percentage >= 0) {
    progress_bar_.set_fraction(p.percentage);
  }
  progress_bar_.set_text(get_progress_str(p));
}


std::string EncodeWindow::get_progress_str(const FFmpegExecutor::Progress& progress)
{
  return Glib::ustring::compose(_("%1%% done, %2"),
                                (int) (progress.percentage * 100),
                                get_time_remaining(progress.seconds_remaining));
}


std::string EncodeWindow::get_time_remaining(int seconds_remaining)
{
  int hours = seconds_remaining/(60*60);
  seconds_remaining %= 60*60;
  int minutes = seconds_remaining/60;
  int seconds = seconds_remaining % 60;

  std::string min_str = Glib::ustring::format(std::setfill(L'0'), std::setw(2), minutes);
  std::string sec_str = Glib::ustring::format(std::setfill(L'0'), std::setw(2), seconds);
  return Glib::ustring::compose(_("about %1:%2:%3 left"), hours, min_str, sec_str);
}


void EncodeWindow::on_ffmpeg_finished(bool success, const std::string& error)
{
  enable_widgets();
  progress_bar_.set_fraction(1);

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


bool EncodeWindow::on_delete_event(GdkEventAny*)
{
  if (!ffmpeg_.is_executing()) {
    return false;
  }

  Gtk::MessageDialog dlg(*this,
                         _("Encoding is in progress. If it is cancelled now, it'll be necessary to restart encoding from the beginning. Really cancel?"),
                         false,
                         Gtk::MESSAGE_QUESTION,
                         Gtk::BUTTONS_NONE);
  dlg.add_button(_("C_ancel encoding"), Gtk::RESPONSE_CLOSE);
  dlg.add_button(_("_Continue"), Gtk::RESPONSE_OK);
  bool terminate = dlg.run() == Gtk::RESPONSE_CLOSE;

  if (terminate) {
    ffmpeg_.terminate();
  }

  return !terminate;
}
