/*
 * Copyright (C) 2018-2019 Werner Turing <werner.turing@protonmail.com>
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


EncodeWindow* EncodeWindow::create(std::unique_ptr<fg::FilterData> filter_data,
                                   int frame_width, int frame_height,
                                   int total_frames, double fps)
{
  auto builder = Gtk::Builder::create_from_resource("/wt/multi-delogo/EncodeWindow.ui");
  EncodeWindow* window = nullptr;
  builder->get_widget_derived("encode_window", window, std::move(filter_data),
                              frame_width, frame_height, total_frames, fps);
  return window;
}


EncodeWindow::EncodeWindow(BaseObjectType* cobject,
                           const Glib::RefPtr<Gtk::Builder>& builder,
                           std::unique_ptr<fg::FilterData> filter_data,
                           int frame_width, int frame_height,
                           int total_frames, double fps)
  : MultiDelogoAppWindow(cobject)
  , filter_data_(std::move(filter_data))
  , frame_width_(frame_width)
  , frame_height_(frame_height)
  , fps_(fps)

  , txt_file_(nullptr)
  , txt_quality_(nullptr)
  , chk_fuzzy_(nullptr)
  , txt_fuzzyness_(nullptr)
  , box_progress_(nullptr)
  , lbl_status_(nullptr)
  , progress_bar_(nullptr)
  , btn_log_(nullptr)
{
  configure_widgets(builder);

  ffmpeg_.set_total_frames(total_frames);
  ffmpeg_.signal_progress().connect(sigc::mem_fun(*progress_bar_, &ETRProgressBar::set_progress));
  ffmpeg_.signal_finished().connect(sigc::mem_fun(*this, &EncodeWindow::on_ffmpeg_finished));
}


void EncodeWindow::configure_widgets(const Glib::RefPtr<Gtk::Builder>& builder)
{
  builder->get_widget("txt_file", txt_file_);
  Gtk::Button* btn_select_file = nullptr;
  builder->get_widget("btn_select_file", btn_select_file);
  btn_select_file->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_select_file));

  Gtk::Grid* grid_file_selection = nullptr;
  builder->get_widget("grid_file_selection", grid_file_selection);
  widgets_to_disable_.push_back(grid_file_selection);

  Gtk::RadioButton* btn_h264 = nullptr;
  builder->get_widget("btn_h264", btn_h264);
  btn_h264->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &EncodeWindow::on_codec),
               FFmpegExecutor::Codec::H264));
  Gtk::RadioButton* btn_h265 = nullptr;
  builder->get_widget("btn_h265", btn_h265);
  btn_h265->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &EncodeWindow::on_codec),
               FFmpegExecutor::Codec::H265));

  Gtk::Grid* grid_codec = nullptr;
  builder->get_widget("grid_codec", grid_codec);
  widgets_to_disable_.push_back(grid_codec);

  builder->get_widget("txt_quality", txt_quality_);

  Gtk::Grid* grid_quality = nullptr;
  builder->get_widget("grid_quality", grid_quality);
  widgets_to_disable_.push_back(grid_quality);

  builder->get_widget("chk_fuzzy", chk_fuzzy_);
  chk_fuzzy_->signal_toggled().connect(sigc::mem_fun(*this, &EncodeWindow::on_fuzzy_toggled));
  builder->get_widget("txt_fuzzyness", txt_fuzzyness_);

  Gtk::Grid* grid_fuzzy = nullptr;
  builder->get_widget("grid_fuzzy", grid_fuzzy);
  widgets_to_disable_.push_back(grid_fuzzy);

  Gtk::Button* btn_script = nullptr;
  builder->get_widget("btn_script", btn_script);
  btn_script->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_generate_script));

  Gtk::Button* btn_encode = nullptr;
  builder->get_widget("btn_encode", btn_encode);
  btn_encode->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_encode));

  Gtk::Grid* grid_buttons = nullptr;
  builder->get_widget("grid_buttons", grid_buttons);
  widgets_to_disable_.push_back(grid_buttons);

  builder->get_widget("lbl_status", lbl_status_);
  builder->get_widget_derived("progress_bar", progress_bar_);
  builder->get_widget("btn_log", btn_log_);
  btn_log_->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_view_log));

  builder->get_widget("grid_progress", box_progress_);
  box_progress_->hide();

  on_codec(FFmpegExecutor::Codec::H264);
}


void EncodeWindow::on_select_file()
{
  Gtk::FileChooserDialog dlg(*this, _("Select output file"), Gtk::FILE_CHOOSER_ACTION_SAVE);
  dlg.add_button(_("_Cancel"), Gtk::RESPONSE_CANCEL);
  dlg.add_button(_("_Save"), Gtk::RESPONSE_OK);
  dlg.set_current_folder(Glib::path_get_dirname(filter_data_->movie_file()));

  if (dlg.run() == Gtk::RESPONSE_OK) {
    txt_file_->set_text(dlg.get_file()->get_path());
  }
}


void EncodeWindow::on_codec(FFmpegExecutor::Codec codec)
{
  codec_ = codec;
  if (codec_ == FFmpegExecutor::Codec::H264) {
    txt_quality_->set_value(FFmpegExecutor::H264_DEFAULT_CRF_);
  } else {
    txt_quality_->set_value(FFmpegExecutor::H265_DEFAULT_CRF_);
  }
}


void EncodeWindow::on_fuzzy_toggled()
{
  txt_fuzzyness_->set_sensitive(chk_fuzzy_->get_active());
}


void EncodeWindow::on_encode()
{
  std::string file = txt_file_->get_text();
  if (!check_file(file)) {
    return;
  }

  Generator generator = get_generator();
  ffmpeg_.set_generator(generator);
  ffmpeg_.set_input_file(filter_data_->movie_file());
  ffmpeg_.set_codec(codec_);
  ffmpeg_.set_quality(txt_quality_->get_value_as_int());
  ffmpeg_.set_output_file(file);

  try {
    ffmpeg_.encode();

    lbl_status_->set_text(_("Encoding in progress"));
    progress_bar_->reset();
    box_progress_->set_no_show_all(false);
    box_progress_->show_all();

    disable_widgets();
    btn_log_->hide();
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
  std::string file = txt_file_->get_text();
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
  if (chk_fuzzy_->get_active()) {
    g = fg::FuzzyScriptGenerator::create(filter_data_->filter_list(), frame_width_, frame_height_, fps_, txt_fuzzyness_->get_value());
  } else {
    g = fg::RegularScriptGenerator::create(filter_data_->filter_list(), frame_width_, frame_height_, fps_);
  }
  return g;
}


void EncodeWindow::on_ffmpeg_finished(bool success, const std::string& error)
{
  enable_widgets();
  btn_log_->show();

  progress_bar_->set_finished();

  if (success) {
    lbl_status_->set_text(_("Encoding finished successfully"));
  } else {
    lbl_status_->set_text(Glib::ustring::compose(_("Encoding failed: %1"), error));
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
  LogWindow* window = LogWindow::create(*this, ffmpeg_.get_log());
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


LogWindow* LogWindow::create(Gtk::Window& parent, const std::string& log)
{
  auto builder = Gtk::Builder::create_from_resource("/wt/multi-delogo/LogWindow.ui");
  LogWindow* window = nullptr;
  builder->get_widget_derived("log_window", window,
                              parent, log);
  return window;
}


LogWindow::LogWindow(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& builder,
                     Gtk::Window& parent, const std::string& log)
  : MultiDelogoAppWindow(cobject)
{
  set_transient_for(parent);

  Gtk::TextView* txt = nullptr;
  builder->get_widget("txt_log", txt);
  txt->get_buffer()->set_text(log);
}
