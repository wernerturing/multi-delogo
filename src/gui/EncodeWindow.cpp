/*
 * Copyright (C) 2018-2026 Werner Turing <werner.turing@protonmail.com>
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

#include <boost/algorithm/string/join.hpp>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/RegularScriptGenerator.hpp"

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
  EncodeWindow* window = Gtk::Builder::get_widget_derived<EncodeWindow>(builder, "encode_window", std::move(filter_data),
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
  , cmb_preset_(nullptr)
  , chk_no_audio_(nullptr)
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
  txt_file_ = builder->get_widget<Gtk::Entry>("txt_file");
  Gtk::Button* btn_select_file = builder->get_widget<Gtk::Button>("btn_select_file");
  btn_select_file->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_select_file));

  Gtk::Box* box_file_selection = builder->get_widget<Gtk::Box>("box_file_selection");
  widgets_to_disable_.push_back(box_file_selection);

  Gtk::RadioButton* btn_h264 = builder->get_widget<Gtk::RadioButton>("btn_h264");
  btn_h264->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &EncodeWindow::on_codec),
               FFmpegExecutor::Codec::H264));
  Gtk::RadioButton* btn_h265 = builder->get_widget<Gtk::RadioButton>("btn_h265");
  btn_h265->signal_toggled().connect(
    sigc::bind(sigc::mem_fun(*this, &EncodeWindow::on_codec),
               FFmpegExecutor::Codec::H265));

  Gtk::Box* box_codec = builder->get_widget<Gtk::Box>("box_codec");
  widgets_to_disable_.push_back(box_codec);

  txt_quality_ = builder->get_widget<Gtk::SpinButton>("txt_quality");
  cmb_preset_ = builder->get_widget<Gtk::ComboBoxText>("cmb_preset");

  Gtk::Box* box_quality = builder->get_widget<Gtk::Box>("box_quality");
  widgets_to_disable_.push_back(box_quality);

  chk_scale_ = builder->get_widget<Gtk::CheckButton>("chk_scale");
  chk_scale_->signal_toggled().connect(sigc::mem_fun(*this, &EncodeWindow::on_scale_toggled));
  txt_scale_width_ = builder->get_widget<Gtk::SpinButton>("txt_scale_width");
  txt_scale_height_ = builder->get_widget<Gtk::SpinButton>("txt_scale_height");

  Gtk::Box* box_scale = builder->get_widget<Gtk::Box>("box_scale");
  widgets_to_disable_.push_back(box_scale);

  chk_no_audio_ = builder->get_widget<Gtk::CheckButton>("chk_no_audio");
  widgets_to_disable_.push_back(chk_no_audio_);

  Gtk::Button* btn_cmd_line = builder->get_widget<Gtk::Button>("btn_cmd_line");
  btn_cmd_line->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_show_cmd_line));

  Gtk::Button* btn_script = builder->get_widget<Gtk::Button>("btn_script");
  btn_script->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_generate_script));

  Gtk::Button* btn_encode = builder->get_widget<Gtk::Button>("btn_encode");
  btn_encode->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_encode));

  Gtk::Box* box_buttons = builder->get_widget<Gtk::Box>("box_buttons");
  widgets_to_disable_.push_back(box_buttons);

  lbl_status_ = builder->get_widget<Gtk::Label>("lbl_status");
  progress_bar_ = Gtk::Builder::get_widget_derived<ETRProgressBar>(builder, "progress_bar");
  btn_log_ = builder->get_widget<Gtk::Button>("btn_log");
  btn_log_->signal_clicked().connect(sigc::mem_fun(*this, &EncodeWindow::on_view_log));

  box_progress_ = builder->get_widget<Gtk::Box>("box_progress");
  box_progress_->set_visible(false);

  on_codec(FFmpegExecutor::Codec::H264);
}


void EncodeWindow::on_select_file()
{
  Gtk::FileChooserDialog dlg(*this, _("Select output file"), Gtk::FileChooser::Action::SAVE);
  dlg.add_button(_("_Cancel"), Gtk::ResponseType::CANCEL);
  dlg.add_button(_("_Save"), Gtk::ResponseType::OK);
  dlg.set_current_folder(Glib::path_get_dirname(filter_data_->movie_file()));

  if (run_dialog_sync(dlg) == Gtk::ResponseType::OK) {
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


void EncodeWindow::on_scale_toggled()
{
  txt_scale_width_->set_sensitive(chk_scale_->get_active());
  txt_scale_height_->set_sensitive(chk_scale_->get_active());
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
  ffmpeg_.set_preset(cmb_preset_->get_active_text());
  ffmpeg_.set_output_file(file);

  try {
    ffmpeg_.encode();

    lbl_status_->set_text(_("Encoding in progress"));
    progress_bar_->reset();
    box_progress_->set_visible();

    disable_widgets();
    btn_log_->set_visible(false);
  } catch (ScriptGenerationException& e) {
    message_dialog(*this,
                   Glib::ustring::compose(_("Error generating filter script for FFmpeg: %1"), e.what()),
                   Gtk::MessageType::ERROR);
  } catch (FFmpegStartException& e) {
    auto msg = Glib::ustring::compose(_("Could not execute FFmpeg: %1"),
                                      e.what());
    message_dialog(*this, msg, Gtk::MessageType::ERROR);
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

    message_dialog(*this, _("Filter script generated"), Gtk::MessageType::INFO);
  } catch (ScriptGenerationException& e) {
    auto msg = Glib::ustring::compose(_("Could not open file %1: %2"),
                                      file, e.what());
    message_dialog(*this, msg, Gtk::MessageType::ERROR);
    return;
  }
}


void EncodeWindow::on_show_cmd_line()
{
  ffmpeg_.set_generator(get_generator());
  ffmpeg_.set_input_file(filter_data_->movie_file());
  ffmpeg_.set_codec(codec_);
  ffmpeg_.set_preset(cmb_preset_->get_active_text());
  ffmpeg_.set_quality(txt_quality_->get_value_as_int());
  ffmpeg_.set_output_file(txt_file_->get_text());

  std::vector<std::string> args = ffmpeg_.get_ffmpeg_cmd_line(_("FILTER_FILE"));
  std::string cmd_line = boost::algorithm::join(args, " ");

  LogWindow* window = LogWindow::create(*this, cmd_line);
  window->set_title(_("FFmpeg command line"));
  get_application()->register_window(window);
}


bool EncodeWindow::check_file(const std::string& file)
{
  if (file.empty()) {
    message_dialog(*this, _("Please select the output file"), Gtk::MessageType::ERROR);
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
  bool scale = chk_scale_->get_active();
  fg::maybe_int scale_width  = scale
    ? boost::make_optional(txt_scale_width_->get_value_as_int())
    : boost::none;
  fg::maybe_int scale_height = scale
    ? boost::make_optional(txt_scale_height_->get_value_as_int())
    : boost::none;

  bool no_audio = chk_no_audio_->get_active();

  return fg::RegularScriptGenerator::create(filter_data_->filter_list(),
                                            frame_width_, frame_height_, fps_,
                                            scale_width, scale_height,
                                            no_audio);
}


void EncodeWindow::on_ffmpeg_finished(bool success, const std::string& error)
{
  enable_widgets();
  btn_log_->set_visible();

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
  // Returning false calls the default handler (which closes the window)
  return confirm_close() == false;
}


bool EncodeWindow::confirm_close()
{
  if (!ffmpeg_.is_executing()) {
    return true;
  }

  bool terminate = confirmation_dialog(*this,
                                       _("Encoding is in progress. If it is cancelled now, it'll be necessary to restart encoding from the beginning. Really cancel?"),
                                       _("C_ancel encoding"), _("_Continue"));

  if (terminate) {
    ffmpeg_.terminate();
  }

  return terminate;
}


LogWindow* LogWindow::create(Gtk::Window& parent, const std::string& log)
{
  auto builder = Gtk::Builder::create_from_resource("/wt/multi-delogo/LogWindow.ui");
  LogWindow* window = Gtk::Builder::get_widget_derived<LogWindow>(builder, "log_window",
                                                                  parent, log);
  return window;
}


LogWindow::LogWindow(BaseObjectType* cobject,
                     const Glib::RefPtr<Gtk::Builder>& builder,
                     Gtk::Window& parent, const std::string& log)
  : MultiDelogoAppWindow(cobject)
{
  set_transient_for(parent);

  Gtk::TextView* txt = builder->get_widget<Gtk::TextView>("txt_log");
  txt->get_buffer()->set_text(log);
}
