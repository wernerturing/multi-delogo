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
#ifndef MDL_EXCEPTIONS_H
#define MDL_EXCEPTIONS_H

#include <string>
#include <exception>


namespace mdl {
  class Exception : public std::exception
  {
  public:
    virtual const char* what() const throw() = 0;
  };


  class VideoNotOpenedException : public Exception
  {
  public:
    const char* what() const throw() override
    {
      return "Failed to open video file";
    }
  };


  class FrameNotAvailableException : public Exception
  {
  public:
    FrameNotAvailableException(int frame)
      : frame_(frame) { }

    int get_frame() const
      { return frame_; }

    const char* what() const throw() override
    {
      return "Failed to get frame";
    }

  private:
    int frame_;
  };


  class DuplicateRowException : public Exception
  {
  public:
    const char* what() const throw() override
    {
      return "Duplicate row";
    }
  };


  class ScriptGenerationException : public Exception
  {
  public:
    ScriptGenerationException(const std::string& msg)
      : msg_(msg) { }

    const char* what() const throw() override
    {
      return msg_.c_str();
    }

  private:
    std::string msg_;
  };


  class FFmpegStartException : public Exception
  {
  public:
    FFmpegStartException(const std::string& msg)
      : msg_(msg) { }

    const char* what() const throw() override
    {
      return msg_.c_str();
    }

  private:
    std::string msg_;
  };
}


#endif // MDL_EXCEPTIONS_H
