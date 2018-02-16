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

#include "../TestHelpers.hpp"


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


BOOST_AUTO_TEST_CASE(filter_panel_null_should_return_a_null_filter)
{
  fg::NullFilter filter;
  FilterPanel* panel = FilterPanelFactory::create(&filter);

  fg::Filter* created_filter = panel->get_filter();
  BOOST_CHECK_EQUAL(created_filter->type(), fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_delogo_filter)
{
  fg::DelogoFilter filter(1, 2, 3, 4);
  FilterPanel* panel = FilterPanelFactory::create(&filter);
  FilterPanelDelogo* downcasted = dynamic_cast<FilterPanelDelogo*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(filter_panel_delogo_should_return_a_delogo_filter)
{
  fg::DelogoFilter filter(1, 2, 3, 4);
  FilterPanel* panel = FilterPanelFactory::create(&filter);

  fg::Filter* created_filter = panel->get_filter();
  BOOST_REQUIRE_EQUAL(created_filter->type(), fg::FilterType::DELOGO);

  fg::DelogoFilter* delogo = dynamic_cast<fg::DelogoFilter*>(created_filter);
  BOOST_CHECK_EQUAL(delogo->x(), 1);
  BOOST_CHECK_EQUAL(delogo->y(), 2);
  BOOST_CHECK_EQUAL(delogo->width(), 3);
  BOOST_CHECK_EQUAL(delogo->height(), 4);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_drawbox_filter)
{
  fg::DrawboxFilter filter(11, 22, 33, 44);
  FilterPanel* panel = FilterPanelFactory::create(&filter);
  FilterPanelDrawbox* downcasted = dynamic_cast<FilterPanelDrawbox*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(filter_panel_drawbox_should_return_a_drawbox_filter)
{
  fg::DrawboxFilter filter(11, 22, 33, 44);
  FilterPanel* panel = FilterPanelFactory::create(&filter);

  fg::Filter* created_filter = panel->get_filter();
  BOOST_REQUIRE_EQUAL(created_filter->type(), fg::FilterType::DRAWBOX);

  fg::DrawboxFilter* drawbox = dynamic_cast<fg::DrawboxFilter*>(created_filter);
  BOOST_CHECK_EQUAL(drawbox->x(), 11);
  BOOST_CHECK_EQUAL(drawbox->y(), 22);
  BOOST_CHECK_EQUAL(drawbox->width(), 33);
  BOOST_CHECK_EQUAL(drawbox->height(), 44);
}
