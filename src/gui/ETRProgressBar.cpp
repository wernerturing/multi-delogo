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
#include <string>
#include <iomanip>

#include <gtkmm.h>
#include <glibmm/i18n.h>

#include "ETRProgressBar.hpp"

using namespace mdl;


void Progress::calculate_time_remaining()
{
  total_seconds_remaining = seconds_elapsed / percentage - seconds_elapsed;
  hours_remaining = total_seconds_remaining / (60*60);
  int remainder = total_seconds_remaining % (60*60);
  minutes_remaining = remainder / 60;
  seconds_remaining = remainder % 60;
}


ETRProgressBar::ETRProgressBar()
{
  set_show_text();
}


ETRProgressBar::ETRProgressBar(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
  : Gtk::ProgressBar(cobject)
{
  set_show_text();
}


void ETRProgressBar::set_progress(const Progress& progress)
{
  if (progress.percentage >= 0) {
    set_fraction(progress.percentage);
    set_text(get_progress_str(progress));
  }
}


void ETRProgressBar::reset()
{
  set_fraction(0);
  set_text("");
}


void ETRProgressBar::set_finished()
{
  set_fraction(1);
  // xgettext:no-c-format
  set_text(_("100% done"));
}


std::string ETRProgressBar::get_progress_str(const Progress& progress)
{
  return Glib::ustring::compose(_("%1%% done, %2"),
                                (int) (progress.percentage * 100),
                                get_time_remaining(progress));
}


std::string ETRProgressBar::get_time_remaining(const Progress& progress)
{
  std::string min_str = Glib::ustring::format(std::setfill(L'0'), std::setw(2),
                                              progress.minutes_remaining);
  std::string sec_str = Glib::ustring::format(std::setfill(L'0'), std::setw(2),
                                              progress.seconds_remaining);
  return Glib::ustring::compose(_("about %1:%2:%3 left"),
                                progress.hours_remaining, min_str, sec_str);
}



