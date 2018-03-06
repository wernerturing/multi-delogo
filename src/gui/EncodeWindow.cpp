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
#include <unistd.h>
#include <cstdio>
#include <cerrno>
#include <memory>
#include <string>
#include <iomanip>
#include <fstream>
#include <regex>

#ifdef __MINGW32__
#  include <windows.h>
#endif

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/RegularScriptGenerator.hpp"
#include "filter-generator/FuzzyScriptGenerator.hpp"

#include "EncodeWindow.hpp"
#include "Utils.hpp"

using namespace mdl;


EncodeWindow::EncodeWindow(std::unique_ptr<fg::FilterData> filter_data, int total_frames)
  : filter_data_(std::move(filter_data))
  , total_frames_(total_frames)
  , codec_(Codec::H264)
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
               Codec::H264));

  Gtk::RadioButton* btn_h265 = Gtk::manage(new Gtk::RadioButton("H.26_5", true));
  btn_h265->join_group(*btn_h264);
  btn_h265->set_tooltip_text(_("A newer format that produces smaller video files. May not work on all players"));
  btn_h265->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &EncodeWindow::on_codec),
               Codec::H265));

  Gtk::Box* box = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_HORIZONTAL, 4));
  box->pack_start(*lbl, false, false);
  box->pack_start(*btn_h264, false, false);
  box->pack_start(*btn_h265, false, false);

  widgets_to_disable_.push_back(box);

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

  try {
    int tmp_fd = Glib::file_open_tmp(tmp_filter_file_, "mdlfilter");
    ::close(tmp_fd);
    Generator generator = get_generator();
    generate_script(tmp_filter_file_, generator);

    std::vector<std::string> cmd_line = get_ffmpeg_cmd_line(tmp_filter_file_, generator);

    start_ffmpeg(cmd_line);
  } catch (Glib::FileError& e) {
    Gtk::MessageDialog dlg(*this,
                           Glib::ustring::compose(_("Error generating filter script for FFmpeg: %1"), e.what()),
                           false, Gtk::MESSAGE_ERROR);
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
  generate_script(file, generator);

  Gtk::MessageDialog dlg(*this, _("Filter script generated"));
  dlg.run();
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
    g = fg::FuzzyScriptGenerator::create(filter_data_->filter_list(), txt_fuzzyness_.get_value());
  }  else {
    g = fg::RegularScriptGenerator::create(filter_data_->filter_list());
  }
  return g;
}


void EncodeWindow::generate_script(const std::string& file, Generator generator)
{
  std::ofstream file_stream(file);
  if (!file_stream.is_open()) {
    auto msg = Glib::ustring::compose(_("Could not open file %1: %2"),
                                      file, Glib::strerror(errno));
    Gtk::MessageDialog dlg(*this, msg, false, Gtk::MESSAGE_ERROR);
    dlg.run();
    return;
  }

  generator->generate_ffmpeg_script(file_stream);
  file_stream.close();
}


std::vector<std::string> EncodeWindow::get_ffmpeg_cmd_line(const std::string& filter_file, Generator generator)
{
  std::string codec_name;
  if (codec_ == Codec::H264) {
    codec_name = "libx264";
  } else if (codec_ == Codec::H265) {
    codec_name = "libx265";
  }

  std::string quality = std::to_string(txt_quality_.get_value_as_int());

  std::vector<std::string> cmd_line;
  cmd_line.push_back("ffmpeg");
  cmd_line.push_back("-y");
  cmd_line.push_back("-v"); cmd_line.push_back("quiet");
  cmd_line.push_back("-stats");

  cmd_line.push_back("-i"); cmd_line.push_back(filter_data_->movie_file());
  cmd_line.push_back("-filter_complex_script"); cmd_line.push_back(filter_file);

  cmd_line.push_back("-map"); cmd_line.push_back("[out_v]");
  cmd_line.push_back("-c:v"); cmd_line.push_back(codec_name);
  cmd_line.push_back("-crf"); cmd_line.push_back(quality);

  std::vector<std::string> audio_opts = get_audio_opts(generator);
  cmd_line.insert(cmd_line.end(), audio_opts.begin(), audio_opts.end());

  cmd_line.push_back(txt_file_.get_text());

  return cmd_line;
}


std::vector<std::string> EncodeWindow::get_audio_opts(Generator generator)
{
  std::vector<std::string> audio_opts;

  if (generator->affects_audio()) {
    audio_opts.push_back("-map"); audio_opts.push_back("[out_a]");
    audio_opts.push_back("-c:a"); audio_opts.push_back("aac");
    audio_opts.push_back("-b:a"); audio_opts.push_back("192k");
  } else {
    audio_opts.push_back("-map"); audio_opts.push_back("0:a?");
    audio_opts.push_back("-c:a"); audio_opts.push_back("copy");
  }

  return audio_opts;
}


void EncodeWindow::start_ffmpeg(const std::vector<std::string>& cmd_line)
{
  Glib::Pid ffmpeg_pid;
  int ffmpeg_stderr_fd;
  try {
    Glib::spawn_async_with_pipes("",
                                 cmd_line,
                                 Glib::SPAWN_SEARCH_PATH | Glib::SPAWN_DO_NOT_REAP_CHILD | Glib::SPAWN_STDOUT_TO_DEV_NULL,
                                 Glib::SlotSpawnChildSetup(),
                                 &ffmpeg_pid,
                                 nullptr,
                                 nullptr,
                                 &ffmpeg_stderr_fd);
  } catch (Glib::SpawnError& e) {
    ::unlink(tmp_filter_file_.c_str());
    auto msg = Glib::ustring::compose(_("Could not execute FFmpeg: %1"),
                                      e.what());
    Gtk::MessageDialog dlg(*this, msg, false, Gtk::MESSAGE_ERROR);
    dlg.run();
    return;
  }

  lbl_status_.set_text(_("Encoding in progress"));
  progress_bar_.set_fraction(0);
  box_progress_.set_no_show_all(false);
  box_progress_.show_all();

  disable_widgets();

  ffmpeg_timer_.start();

  Glib::signal_child_watch().connect(sigc::mem_fun(*this, &EncodeWindow::on_ffmpeg_finished),
                                     ffmpeg_pid);

  ffmpeg_out_ = Glib::IOChannel::create_from_fd(ffmpeg_stderr_fd);
  const auto io_source = Glib::IOSource::create(ffmpeg_out_,
                                                Glib::IO_IN | Glib::IO_HUP);
  io_source->set_priority(Glib::PRIORITY_LOW);
  io_source->connect(sigc::mem_fun(*this, &EncodeWindow::on_ffmpeg_output));
  io_source->attach(Glib::MainContext::get_default());

#ifdef __MINGW32__
  ffmpeg_handle_ = ffmpeg_pid;
#endif
}


bool EncodeWindow::on_ffmpeg_output(Glib::IOCondition condition)
{
  // Under windows this function gets called after the process has terminated
  // and the variable has been cleared
  if (!ffmpeg_out_) {
    return false;
  }

  if (condition == Glib::IO_HUP) {
    ffmpeg_out_.reset();
    return false;
  }

  Glib::ustring line;
  ffmpeg_out_->read_line(line);
  auto last_char = line.size() - 1;
  if (line[last_char] == '\r' || line[last_char] == '\n') {
    line.erase(last_char);
  }

  Progress p = get_progress(line);
  if (p.percentage >= 0) {
    progress_bar_.set_fraction(p.percentage);
  }
  progress_bar_.set_text(get_progress_str(p));

  return true;
}


EncodeWindow::Progress EncodeWindow::get_progress(const std::string& ffmpeg_stats)
{
  Progress p;

  std::regex r("^frame=\\s+(\\d+)");
  std::smatch matches;
  if (std::regex_search(ffmpeg_stats, matches, r)) {
    int frames_encoded = std::stoi(matches[1].str());
    p.percentage = (double) frames_encoded / total_frames_;
  } else {
    p.percentage = -1;
  }

  p.seconds_elapsed = ffmpeg_timer_.elapsed();

  return p;
}


std::string EncodeWindow::get_progress_str(const Progress& progress)
{
  return Glib::ustring::compose(_("%1%% done, %2"),
                                (int) (progress.percentage * 100),
                                get_time_remaining(calculate_seconds_remaining(progress)));
}


int EncodeWindow::calculate_seconds_remaining(const Progress& progress)
{
  return progress.seconds_elapsed / progress.percentage - progress.seconds_elapsed;
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


void EncodeWindow::on_ffmpeg_finished(Glib::Pid pid, int status)
{
  Glib::spawn_close_pid(pid);
  ::unlink(tmp_filter_file_.c_str());
  ffmpeg_out_.reset();

  enable_widgets();
  progress_bar_.set_fraction(1);

  GError *error = nullptr;
  if (g_spawn_check_exit_status(status, &error)) {
    lbl_status_.set_text(_("Encoding finished successfully"));
  } else {
    lbl_status_.set_text(Glib::ustring::compose(_("Encoding failed: %1"), error->message));
    g_error_free(error);
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
  if (!ffmpeg_out_) {
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

#ifdef __MINGW32__
  if (terminate) {
    TerminateProcess(ffmpeg_handle_, 250);
  }
#endif

  return !terminate;
}
