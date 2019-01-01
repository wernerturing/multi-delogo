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
#include <gtkmm.h>

#include "filter-generator/Filters.hpp"

#include "FilterPanelFactory.hpp"
#include "FilterPanels.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE FilterPanelFactory
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


class Fixture
{
public:
  Fixture()
    : factory(5000, 1000, 1000)
  {
  }

  FilterPanelFactory factory;
};


BOOST_FIXTURE_TEST_SUITE(Panel_creation, Fixture)

BOOST_AUTO_TEST_CASE(should_create_a_panel_for_null_filter)
{
  fg::filter_ptr filter(new fg::NullFilter);
  FilterPanel* panel = factory.create(1, filter);
  FilterPanelNull* downcasted = dynamic_cast<FilterPanelNull*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_null_filter_from_type)
{
  FilterPanel* panel = factory.create(1, fg::FilterType::NO_OP);
  FilterPanelNull* downcasted = dynamic_cast<FilterPanelNull*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(filter_panel_null_should_return_a_null_filter)
{
  fg::filter_ptr filter(new fg::NullFilter);
  FilterPanel* panel = factory.create(1, filter);

  fg::filter_ptr created_filter = panel->get_filter();
  BOOST_CHECK_EQUAL(created_filter->type(), fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(filter_panel_null_should_return_no_rectangle)
{
  fg::filter_ptr filter(new fg::NullFilter);
  FilterPanel* panel = factory.create(1, filter);

  auto rect = panel->get_rectangle();
  BOOST_CHECK(!rect);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_delogo_filter)
{
  fg::filter_ptr filter(new fg::DelogoFilter(1, 2, 3, 4));
  FilterPanel* panel = factory.create(1, filter);
  FilterPanelDelogo* downcasted = dynamic_cast<FilterPanelDelogo*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_delogo_filter_from_type)
{
  FilterPanel* panel = factory.create(1, fg::FilterType::DELOGO);
  FilterPanelDelogo* downcasted = dynamic_cast<FilterPanelDelogo*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(filter_panel_delogo_should_return_a_delogo_filter)
{
  fg::filter_ptr filter(new fg::DelogoFilter(1, 2, 3, 4));
  FilterPanel* panel = factory.create(1, filter);

  fg::filter_ptr created_filter = panel->get_filter();
  BOOST_REQUIRE_EQUAL(created_filter->type(), fg::FilterType::DELOGO);

  fg::DelogoFilter* delogo = dynamic_cast<fg::DelogoFilter*>(created_filter.get());
  BOOST_CHECK_EQUAL(delogo->x(), 1);
  BOOST_CHECK_EQUAL(delogo->y(), 2);
  BOOST_CHECK_EQUAL(delogo->width(), 3);
  BOOST_CHECK_EQUAL(delogo->height(), 4);
}


BOOST_AUTO_TEST_CASE(filter_panel_delogo_should_return_a_rectangle)
{
  fg::filter_ptr filter(new fg::DelogoFilter(15, 20, 80, 40));
  FilterPanel* panel = factory.create(1, filter);

  auto rect = panel->get_rectangle();
  BOOST_REQUIRE(rect);
  BOOST_CHECK_EQUAL(rect->x, 15);
  BOOST_CHECK_EQUAL(rect->y, 20);
  BOOST_CHECK_EQUAL(rect->width, 80);
  BOOST_CHECK_EQUAL(rect->height, 40);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_drawbox_filter)
{
  fg::filter_ptr filter(new fg::DrawboxFilter(11, 22, 33, 44));
  FilterPanel* panel = factory.create(1, filter);
  FilterPanelDrawbox* downcasted = dynamic_cast<FilterPanelDrawbox*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_drawbox_filter_from_type)
{
  FilterPanel* panel = factory.create(1, fg::FilterType::DRAWBOX);
  FilterPanelDrawbox* downcasted = dynamic_cast<FilterPanelDrawbox*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(filter_panel_drawbox_should_return_a_drawbox_filter)
{
  fg::filter_ptr filter(new fg::DrawboxFilter(11, 22, 33, 44));
  FilterPanel* panel = factory.create(1, filter);

  fg::filter_ptr created_filter = panel->get_filter();
  BOOST_REQUIRE_EQUAL(created_filter->type(), fg::FilterType::DRAWBOX);

  fg::DrawboxFilter* drawbox = dynamic_cast<fg::DrawboxFilter*>(created_filter.get());
  BOOST_CHECK_EQUAL(drawbox->x(), 11);
  BOOST_CHECK_EQUAL(drawbox->y(), 22);
  BOOST_CHECK_EQUAL(drawbox->width(), 33);
  BOOST_CHECK_EQUAL(drawbox->height(), 44);
}


BOOST_AUTO_TEST_CASE(filter_panel_drawbox_should_return_a_rectangle)
{
  fg::filter_ptr filter(new fg::DrawboxFilter(0, 500, 100, 30));
  FilterPanel* panel = factory.create(1, filter);

  auto rect = panel->get_rectangle();
  BOOST_REQUIRE(rect);
  BOOST_CHECK_EQUAL(rect->x, 0);
  BOOST_CHECK_EQUAL(rect->y, 500);
  BOOST_CHECK_EQUAL(rect->width, 100);
  BOOST_CHECK_EQUAL(rect->height, 30);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_cut_filter)
{
  fg::filter_ptr filter(new fg::CutFilter());
  FilterPanel* panel = factory.create(1, filter);
  FilterPanelCut* downcasted = dynamic_cast<FilterPanelCut*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_cut_filter_from_type)
{
  FilterPanel* panel = factory.create(1, fg::FilterType::CUT);
  FilterPanelCut* downcasted = dynamic_cast<FilterPanelCut*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(filter_panel_cut_should_return_a_cut_filter)
{
  fg::filter_ptr filter(new fg::CutFilter());
  FilterPanel* panel = factory.create(1, filter);

  fg::filter_ptr created_filter = panel->get_filter();
  BOOST_CHECK_EQUAL(created_filter->type(), fg::FilterType::CUT);
}


BOOST_AUTO_TEST_CASE(filter_panel_cut_should_return_no_rectangle)
{
  fg::filter_ptr filter(new fg::CutFilter());
  FilterPanel* panel = factory.create(1, filter);

  auto rect = panel->get_rectangle();
  BOOST_CHECK(!rect);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_review_filter)
{
  fg::filter_ptr filter(new fg::ReviewFilter());
  FilterPanel* panel = factory.create(1, filter);
  FilterPanelReview* downcasted = dynamic_cast<FilterPanelReview*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(should_create_a_panel_for_review_filter_from_type)
{
  FilterPanel* panel = factory.create(1, fg::FilterType::REVIEW);
  FilterPanelReview* downcasted = dynamic_cast<FilterPanelReview*>(panel);

  BOOST_CHECK(downcasted != nullptr);
}


BOOST_AUTO_TEST_CASE(filter_panel_review_should_return_a_review_filter)
{
  fg::filter_ptr filter(new fg::ReviewFilter());
  FilterPanel* panel = factory.create(1, filter);

  fg::filter_ptr created_filter = panel->get_filter();
  BOOST_CHECK_EQUAL(created_filter->type(), fg::FilterType::REVIEW);
}


BOOST_AUTO_TEST_CASE(filter_panel_review_should_return_no_rectangle)
{
  fg::filter_ptr filter(new fg::ReviewFilter());
  FilterPanel* panel = factory.create(1, filter);

  auto rect = panel->get_rectangle();
  BOOST_CHECK(!rect);
}

BOOST_AUTO_TEST_SUITE_END()
