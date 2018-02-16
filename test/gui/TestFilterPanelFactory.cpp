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
#include <gtkmm.h>

#include "filter-generator/Filters.hpp"

#include "FilterPanelFactory.hpp"
#include "FilterPanels.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE Filter panel factory
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>


class GtkInitialization
{
public:
  GtkInitialization()
  {
    Gtk::Main();
  }
};
BOOST_GLOBAL_FIXTURE(GtkInitialization);


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_null_filter)
{
  fg::NullFilter filter;
  FilterPanel* panel = FilterPanelFactory::create(&filter);
  FilterPanelNull* downcasted = dynamic_cast<FilterPanelNull*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}
