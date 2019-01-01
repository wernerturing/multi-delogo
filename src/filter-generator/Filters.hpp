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
#ifndef FG_FILTERS_H
#define FG_FILTERS_H

#include <memory>
#include <string>
#include <vector>
#include <ostream>


namespace fg {
  enum class FilterType
  {
    NO_OP,
    DELOGO,
    DRAWBOX,
    CUT,
    REVIEW,
  };


  class Filter
  {
  public:
    virtual ~Filter();

    virtual FilterType type() const = 0;
    virtual std::string name() const = 0;

    virtual bool affects_audio() const;

    virtual std::string save_str() const = 0;
    virtual std::string ffmpeg_str(const std::string& between_expr,
                                   int frame_width, int frame_height) const = 0;
  };


  typedef std::shared_ptr<Filter> filter_ptr;


  class NullFilter : public Filter
  {
  public:
    static std::shared_ptr<NullFilter> load(const std::string& parameters);

    FilterType type() const override;
    std::string name() const override;

    std::string save_str() const override;
    std::string ffmpeg_str(const std::string& between_expr,
                           int frame_width, int frame_height) const override;
  };


  class RectangularFilter : public Filter
  {
  protected:
    RectangularFilter(int x, int y, int width, int height);

  public:
    int x() const;
    int y() const;
    int width() const;
    int height() const;

  protected:
    static void load_rectangle(const std::string& parameters,
                               int& x, int& y, int& width, int& height);
    std::string rectangle_save_str() const;
    std::string rectangle_ffmpeg_str() const;
    std::string rectangle_ffmpeg_str(int x, int y, int width, int height) const;

  private:
    int x_;
    int y_;
    int width_;
    int height_;
  };


  class DelogoFilter : public RectangularFilter
  {
  public:
    DelogoFilter(int x, int y, int width, int height);

    static std::shared_ptr<DelogoFilter> load(const std::string& parameters);

    FilterType type() const override;
    std::string name() const override;

    std::string save_str() const override;
    std::string ffmpeg_str(const std::string& between_expr,
                           int frame_width, int frame_height) const override;
  };


  class DrawboxFilter : public RectangularFilter
  {
  public:
    DrawboxFilter(int x, int y, int width, int height);

    static std::shared_ptr<DrawboxFilter> load(const std::string& parameters);

    FilterType type() const override;
    std::string name() const override;

    std::string save_str() const override;
    std::string ffmpeg_str(const std::string& between_expr,
                           int frame_width, int frame_height) const override;
  };


  class CutFilter : public Filter
  {
  public:
    static std::shared_ptr<CutFilter> load(const std::string& parameters);

    FilterType type() const override;
    std::string name() const override;

    bool affects_audio() const override;

    std::string save_str() const override;
    std::string ffmpeg_str(const std::string& between_expr,
                           int frame_width, int frame_height) const override;
  };


  class ReviewFilter : public Filter
  {
  public:
    static std::shared_ptr<ReviewFilter> load(const std::string& parameters);

    FilterType type() const override;
    std::string name() const override;

    std::string save_str() const override;
    std::string ffmpeg_str(const std::string& between_expr,
                           int frame_width, int frame_height) const override;
  };
}

#endif // FG_FILTERS_H
