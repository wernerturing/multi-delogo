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
#ifndef MDL_LOGO_FINDER_H
#define MDL_LOGO_FINDER_H


namespace mdl {
  class LogoFinderResult
  {
  public:
    int start_frame;
    int x;
    int y;
    int width;
    int height;
  };


  class LogoFinderCallback
  {
  public:
    virtual bool success(const LogoFinderResult& result) = 0;
    virtual bool failure(int start_frame) = 0;
  };


  class LogoFinder
  {
  public:
    LogoFinder(LogoFinderCallback& callback)
      : callback_(callback) { };
    virtual ~LogoFinder() { };

    virtual void find_logos() = 0;

  protected:
    LogoFinderCallback& callback_;
  };
}


#endif // MDL_LOGO_FINDER_H
