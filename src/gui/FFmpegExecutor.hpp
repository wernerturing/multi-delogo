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
#ifndef MDL_FFMPEG_EXECUTOR_H
#define MDL_FFMPEG_EXECUTOR_H

#include <memory>
#include <string>
#include <vector>

#include <glibmm.h>

#include "filter-generator/ScriptGenerator.hpp"

#include "ETRProgressBar.hpp"


namespace mdl {
  class FFmpegExecutor
  {
  public:
    enum class Codec { H264, H265 };
    static const int H264_DEFAULT_CRF_ = 23;
    static const int H265_DEFAULT_CRF_ = 28;

    typedef std::shared_ptr<fg::ScriptGenerator> Generator;

  public:
    void set_generator(Generator generator);

    void set_input_file(const std::string& input_file);
    void set_total_frames(int total_frames);

    void set_codec(Codec codec);
    void set_quality(int quality);
    void set_output_file(const std::string& output_file);

    void encode();
    void generate_script(const std::string& output_script);

    bool is_executing() const;
    void terminate();

    const std::string& get_log() const;

    typedef sigc::signal<void, Progress> type_signal_progress;
    type_signal_progress signal_progress();

    typedef sigc::signal<void, bool, std::string> type_signal_finished;
    type_signal_finished signal_finished();

  private:
    Generator generator_;

    std::string input_file_;
    int total_frames_;

    Codec codec_;
    int quality_;
    std::string output_file_;

    std::string tmp_filter_file_;
    int total_frames_output_;
    Glib::Pid ffmpeg_pid_;
    Glib::RefPtr<Glib::IOChannel> ffmpeg_out_;
    sigc::connection ffmpeg_out_signal_;
    Glib::Timer ffmpeg_timer_;

    std::string log_;

    type_signal_progress signal_progress_;
    type_signal_finished signal_finished_;


    std::vector<std::string> get_ffmpeg_cmd_line(const std::string& filter_file);
    bool is_mp4_output() const;
    std::vector<std::string> get_audio_opts();

    void start_ffmpeg(const std::vector<std::string>& cmd_line);

    bool on_ffmpeg_output(Glib::IOCondition condition);
    Progress get_progress(const std::string& ffmpeg_stats);

    void on_ffmpeg_finished(Glib::Pid pid, int status);


    friend class FFmpegExecutorTestFixture;
  };
}

#endif // MDL_FFMPEG_EXECUTOR_H
