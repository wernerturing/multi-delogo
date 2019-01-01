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
#ifndef MDL_FRAME_PROVIDER_H
#define MDL_FRAME_PROVIDER_H

#include <string>

#include <glibmm/objectbase.h>
#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>


namespace mdl {
  class FrameProvider : public Glib::Object
  {
  public:
    virtual Glib::RefPtr<Gdk::Pixbuf> get_frame(int frame_number) = 0;

    virtual int get_frame_width() = 0;
    virtual int get_frame_height() = 0;
    virtual int get_number_of_frames() = 0;
    virtual double get_fps() = 0;
  };


  Glib::RefPtr<FrameProvider> create_frame_provider(const std::string& movie_filename);
}


#endif // MDL_FRAME_PROVIDER_H
