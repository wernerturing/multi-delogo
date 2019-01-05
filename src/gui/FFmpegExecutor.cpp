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
#include <unistd.h>
#include <cerrno>
#include <memory>
#include <string>
#include <fstream>
#include <regex>

#ifndef __MINGW32__
#  include <sys/types.h>
#  include <signal.h>
#else
#  include <windows.h>
#endif

#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/join.hpp>

#include <glibmm.h>

#include "filter-generator/ScriptGenerator.hpp"

#include "common/Exceptions.hpp"
#include "ETRProgressBar.hpp"
#include "FFmpegExecutor.hpp"

using namespace mdl;


void FFmpegExecutor::set_generator(Generator generator)
{
  generator_ = generator;
}


void FFmpegExecutor::set_input_file(const std::string& input_file)
{
  input_file_ = input_file;
}


void FFmpegExecutor::set_total_frames(int total_frames)
{
  total_frames_ = total_frames;
}


void FFmpegExecutor::set_codec(Codec codec)
{
  codec_ = codec;
}


void FFmpegExecutor::set_quality(int quality)
{
  quality_ = quality;
}


void FFmpegExecutor::set_output_file(const std::string& output_file)
{
  output_file_ = output_file;
}


void FFmpegExecutor::encode()
{
  try {
    int tmp_fd = Glib::file_open_tmp(tmp_filter_file_, "mdlfilter");
    ::close(tmp_fd);
  } catch (Glib::FileError& e) {
    throw ScriptGenerationException(e.what());
  }

  generate_script(tmp_filter_file_);

  total_frames_output_ = generator_->resulting_frames(total_frames_);

  std::vector<std::string> cmd_line = get_ffmpeg_cmd_line(tmp_filter_file_);

  start_ffmpeg(cmd_line);
}


void FFmpegExecutor::generate_script(const std::string& output_script)
{
  std::ofstream file_stream(output_script);
  if (!file_stream.is_open()) {
    throw ScriptGenerationException(Glib::strerror(errno));
  }

  generator_->generate_ffmpeg_script(file_stream);
  file_stream.close();
}


bool FFmpegExecutor::is_executing() const
{
  return !!ffmpeg_out_;
}


void FFmpegExecutor::terminate()
{
  ffmpeg_out_signal_.disconnect();
#ifndef __MINGW32__
  kill(ffmpeg_pid_, SIGTERM);
#else
  TerminateProcess(ffmpeg_pid_, 250);
#endif
}


const std::string& FFmpegExecutor::get_log() const
{
  return log_;
}


std::vector<std::string> FFmpegExecutor::get_ffmpeg_cmd_line(const std::string& filter_file)
{
  std::string codec_name;
  if (codec_ == Codec::H264) {
    codec_name = "libx264";
  } else if (codec_ == Codec::H265) {
    codec_name = "libx265";
  }

  std::string quality_str = std::to_string(quality_);

  std::vector<std::string> cmd_line;
  cmd_line.push_back("ffmpeg");
  cmd_line.push_back("-y");
  cmd_line.push_back("-v"); cmd_line.push_back("error");
  cmd_line.push_back("-stats");

  cmd_line.push_back("-i"); cmd_line.push_back(input_file_);
  cmd_line.push_back("-filter_complex_script"); cmd_line.push_back(filter_file);

  cmd_line.push_back("-map"); cmd_line.push_back("[out_v]");
  cmd_line.push_back("-c:v"); cmd_line.push_back(codec_name);
  cmd_line.push_back("-crf"); cmd_line.push_back(quality_str);

  std::vector<std::string> audio_opts = get_audio_opts();
  cmd_line.insert(cmd_line.end(), audio_opts.begin(), audio_opts.end());

  if (is_mp4_output()) {
    cmd_line.push_back("-movflags"); cmd_line.push_back("+faststart");
  }

  cmd_line.push_back(output_file_);

  return cmd_line;
}


std::vector<std::string> FFmpegExecutor::get_audio_opts()
{
  std::vector<std::string> audio_opts;

  if (generator_->affects_audio()) {
    audio_opts.push_back("-map"); audio_opts.push_back("[out_a]");
    audio_opts.push_back("-c:a"); audio_opts.push_back("aac");
    audio_opts.push_back("-b:a"); audio_opts.push_back("192k");
  } else {
    audio_opts.push_back("-map"); audio_opts.push_back("0:a?");
    audio_opts.push_back("-c:a"); audio_opts.push_back("copy");
  }

  return audio_opts;
}


bool FFmpegExecutor::is_mp4_output() const
{
  return boost::algorithm::ends_with(output_file_, ".mp4");
}


void FFmpegExecutor::start_ffmpeg(const std::vector<std::string>& cmd_line)
{
  log_ = boost::algorithm::join(cmd_line, " ");
  log_ += "\n\n";

  int ffmpeg_stderr_fd;
  try {
    Glib::spawn_async_with_pipes("",
                                 cmd_line,
                                 Glib::SPAWN_SEARCH_PATH | Glib::SPAWN_DO_NOT_REAP_CHILD | Glib::SPAWN_STDOUT_TO_DEV_NULL,
                                 Glib::SlotSpawnChildSetup(),
                                 &ffmpeg_pid_,
                                 nullptr,
                                 nullptr,
                                 &ffmpeg_stderr_fd);
  } catch (Glib::SpawnError& e) {
    ::unlink(tmp_filter_file_.c_str());
    throw FFmpegStartException(e.what());
  }

  ffmpeg_timer_.start();

  Glib::signal_child_watch().connect(sigc::mem_fun(*this, &FFmpegExecutor::on_ffmpeg_finished),
                                     ffmpeg_pid_);

  ffmpeg_out_ = Glib::IOChannel::create_from_fd(ffmpeg_stderr_fd);
  const auto io_source = Glib::IOSource::create(ffmpeg_out_,
                                                Glib::IO_IN | Glib::IO_HUP);
  io_source->set_priority(Glib::PRIORITY_LOW);
  ffmpeg_out_signal_ = io_source->connect(sigc::mem_fun(*this, &FFmpegExecutor::on_ffmpeg_output));
  io_source->attach(Glib::MainContext::get_default());
}


bool FFmpegExecutor::on_ffmpeg_output(Glib::IOCondition condition)
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
  signal_progress_.emit(p);

  if (p.percentage < 0) {
    log_ += line;
    log_ += '\n';
  }

  return true;
}


Progress FFmpegExecutor::get_progress(const std::string& ffmpeg_stats)
{
  Progress p;

  std::regex r("^frame=\\s+(\\d+)");
  std::smatch matches;
  if (!std::regex_search(ffmpeg_stats, matches, r)) {
    p.percentage = -1;
    return p;
  }

  int frames_encoded = std::stoi(matches[1].str());
  p.percentage = (double) frames_encoded / total_frames_output_;

  p.seconds_elapsed = ffmpeg_timer_.elapsed();
  p.calculate_time_remaining();

  return p;
}


void FFmpegExecutor::on_ffmpeg_finished(Glib::Pid pid, int status)
{
  Glib::spawn_close_pid(pid);
  ::unlink(tmp_filter_file_.c_str());
  ffmpeg_out_.reset();

  GError *error = nullptr;
  if (g_spawn_check_exit_status(status, &error)) {
    signal_finished_.emit(true, "");
  } else {
    std::string message(error->message);
    g_error_free(error);
    signal_finished_.emit(false, message);
  }
}


FFmpegExecutor::type_signal_progress FFmpegExecutor::signal_progress()
{
  return signal_progress_;
}


FFmpegExecutor::type_signal_finished FFmpegExecutor::signal_finished()
{
  return signal_finished_;
}
