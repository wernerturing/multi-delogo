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
#include <goocanvasmm.h>

#include "FrameView.hpp"

using namespace mdl;


#define BOOST_TEST_MODULE SelectionRect
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

BOOST_TEST_DONT_PRINT_LOG_VALUE(DragMode)


class GtkInitialization
{
public:
  GtkInitialization()
  {
    Gtk::Main();
  }
};
BOOST_GLOBAL_FIXTURE(GtkInitialization);


namespace mdl {
class SelectionRectTestFixture
{
public:
  SelectionRectTestFixture()
  {
    rect = SelectionRect::create(10, 15, 100, 50);
  }

  DragMode get_drag_mode()
  {
    return rect->drag_mode_;
  }

  Point to_inside_coordinates(Point point)
  {
    return rect->to_inside_coordinates(point);
  }

  DragMode get_drag_mode_for_point(Point point)
  {
    return rect->get_drag_mode_for_point(point);
  }

  void start_drag(DragMode mode, Point start)
  {
    rect->start_drag(mode, start);
  };

  Rectangle get_new_coordinates(Point drag_point)
  {
    return rect->get_new_coordinates(drag_point);
  };

  Glib::RefPtr<SelectionRect> rect;
};
}
BOOST_FIXTURE_TEST_SUITE(SelectionRect, mdl::SelectionRectTestFixture);


BOOST_AUTO_TEST_CASE(starts_with_no_drag_mode)
{
  BOOST_CHECK_EQUAL(get_drag_mode(), DragMode::NONE);
}


BOOST_AUTO_TEST_CASE(conversion_to_coordinates_inside_item,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  Point ret = to_inside_coordinates({.x = 21, .y = 23});
  BOOST_CHECK_EQUAL(ret.x, 11.0);
  BOOST_CHECK_EQUAL(ret.y, 8.0);
}


BOOST_AUTO_TEST_CASE(should_return_resize_br_for_the_bottom_right_10x10_square)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 90, .y = 40}),
                    DragMode::RESIZE_BR);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 100, .y = 50}),
                    DragMode::RESIZE_BR);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 95, .y = 43}),
                    DragMode::RESIZE_BR);

  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 89, .y = 43}),
                    DragMode::MOVE);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 97, .y = 39}),
                    DragMode::MOVE);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 12, .y = 21}),
                    DragMode::MOVE);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_move,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::MOVE, {.x = 40, .y = 32});

  Rectangle ret = get_new_coordinates({.x = 67, .y = 29});
  BOOST_CHECK_EQUAL(ret.x, 37.0);
  BOOST_CHECK_EQUAL(ret.y, 12.0);
  BOOST_CHECK_EQUAL(ret.width, 100.0);
  BOOST_CHECK_EQUAL(ret.height, 50.0);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_br,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_BR, {.x = 104, .y = 57});

  Rectangle ret = get_new_coordinates({.x = 100, .y = 79});
  BOOST_CHECK_EQUAL(ret.x, 10.0);
  BOOST_CHECK_EQUAL(ret.y, 15.0);
  BOOST_CHECK_EQUAL(ret.width, 96.0);
  BOOST_CHECK_EQUAL(ret.height, 72.0);
}

BOOST_AUTO_TEST_SUITE_END()
