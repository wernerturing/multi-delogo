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
#ifndef MDL_ETR_PROGRESS_BAR_H
#define MDL_ETR_PROGRESS_BAR_H

#include <string>

#include <gtkmm.h>


namespace mdl {
  class Progress
  {
  public:
    double percentage;
    int seconds_elapsed;
    int total_seconds_remaining;
    int hours_remaining;
    int minutes_remaining;
    int seconds_remaining;

    void calculate_time_remaining();
  };

  class ETRProgressBar : public Gtk::ProgressBar
  {
  public:
    ETRProgressBar();
    ETRProgressBar(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);

    void set_progress(const Progress& progress);
    void reset();
    void set_finished();

  private:
    std::string get_progress_str(const Progress& progress);
    std::string get_time_remaining(const Progress& progress);


    friend class ETRProgressBarTestFixture;
  };
}

#endif // MDL_ETR_PROGRESS_BAR_H
