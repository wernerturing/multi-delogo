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
#include <goocanvasmm.h>

#include "common/Rectangle.hpp"

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

  Point to_inside_coordinates(const Point& point)
  {
    return rect->to_inside_coordinates(point);
  }

  Rectangle normalize(const Rectangle& original)
  {
    return rect->normalize(original);
  }

  DragMode get_drag_mode_for_point(const Point& point)
  {
    return rect->get_drag_mode_for_point(point);
  }

  void start_drag(DragMode mode, const Point& start)
  {
    rect->start_drag(mode, start);
  };

  Rectangle get_new_coordinates(const Point& drag_point)
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


BOOST_AUTO_TEST_CASE(normalize_should_return_same_object_if_coordinates_are_ok)
{
  Rectangle original = {.x = 10, .y = 20, .width = 100, .height = 50};

  Rectangle normalized = normalize(original);
  BOOST_TEST(normalized.x == original.x);
  BOOST_TEST(normalized.y == original.y);
  BOOST_TEST(normalized.width == original.width);
  BOOST_TEST(normalized.height == original.height);
}


BOOST_AUTO_TEST_CASE(normalize_should_adjust_x_if_dragging_to_the_left)
{
  Rectangle original = {.x = 100, .y = 20, .width = -50, .height = 30};

  Rectangle normalized = normalize(original);
  BOOST_TEST(normalized.x == 50);
  BOOST_TEST(normalized.y == original.y);
  BOOST_TEST(normalized.width == 50);
  BOOST_TEST(normalized.height == original.height);
}


BOOST_AUTO_TEST_CASE(normalize_should_adjust_y_if_dragging_to_the_top)
{
  Rectangle original = {.x = 50, .y = 130, .width = 100, .height = -23};

  Rectangle normalized = normalize(original);
  BOOST_TEST(normalized.x == original.x);
  BOOST_TEST(normalized.y == 107);
  BOOST_TEST(normalized.width == 100);
  BOOST_TEST(normalized.height == 23);
}


BOOST_AUTO_TEST_CASE(normalize_should_adjust_x_and_y_if_dragging_to_the_top_left)
{
  Rectangle original = {.x = 200, .y = 150, .width = -115, .height = -30};

  Rectangle normalized = normalize(original);
  BOOST_TEST(normalized.x == 85);
  BOOST_TEST(normalized.y == 120);
  BOOST_TEST(normalized.width == 115);
  BOOST_TEST(normalized.height == 30);
}


BOOST_AUTO_TEST_CASE(should_return_resize_br_for_the_bottom_right_10x10_square)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 90, .y = 40}),
                    DragMode::RESIZE_BR);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 100, .y = 50}),
                    DragMode::RESIZE_BR);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 95, .y = 43}),
                    DragMode::RESIZE_BR);
}


BOOST_AUTO_TEST_CASE(should_return_resize_bl_for_the_bottom_left_10x10_square)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 1, .y = 50}),
                    DragMode::RESIZE_BL);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 10, .y = 40}),
                    DragMode::RESIZE_BL);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 4, .y = 43}),
                    DragMode::RESIZE_BL);
}


BOOST_AUTO_TEST_CASE(should_return_resize_tl_for_the_top_left_10x10_square)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 1, .y = 10}),
                    DragMode::RESIZE_TL);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 10, .y = 2}),
                    DragMode::RESIZE_TL);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 4, .y = 10}),
                    DragMode::RESIZE_TL);
}


BOOST_AUTO_TEST_CASE(should_return_resize_tr_for_the_top_right_10x10_square)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 90, .y = 10}),
                    DragMode::RESIZE_TR);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 100, .y = 0}),
                    DragMode::RESIZE_TR);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 98, .y = 5}),
                    DragMode::RESIZE_TR);
}


BOOST_AUTO_TEST_CASE(should_return_resize_b_for_the_bottom_10_rectangle)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 11, .y = 40}),
                    DragMode::RESIZE_B);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 53, .y = 50}),
                    DragMode::RESIZE_B);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 89, .y = 45}),
                    DragMode::RESIZE_B);
}


BOOST_AUTO_TEST_CASE(should_return_resize_l_for_the_left_10_rectangle)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 1, .y = 11}),
                    DragMode::RESIZE_L);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 10, .y = 26}),
                    DragMode::RESIZE_L);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 3, .y = 39}),
                    DragMode::RESIZE_L);
}


BOOST_AUTO_TEST_CASE(should_return_resize_l_for_the_top_10_rectangle)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 11, .y = 1}),
                    DragMode::RESIZE_T);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 86, .y = 7}),
                    DragMode::RESIZE_T);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 89, .y = 9}),
                    DragMode::RESIZE_T);
}


BOOST_AUTO_TEST_CASE(should_return_resize_r_for_the_right_10_rectangle)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 90, .y = 11}),
                    DragMode::RESIZE_R);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 95, .y = 22}),
                    DragMode::RESIZE_R);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 99, .y = 39}),
                    DragMode::RESIZE_R);
}


BOOST_AUTO_TEST_CASE(should_return_move_for_the_inside_of_the_rectangle)
{
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 89, .y = 11}),
                    DragMode::MOVE);
  BOOST_CHECK_EQUAL(get_drag_mode_for_point({.x = 20, .y = 39}),
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


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_bl,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_BL, {.x = 17, .y = 65});

  Rectangle ret = get_new_coordinates({.x = 8, .y = 70});
  BOOST_CHECK_EQUAL(ret.x, 1.0);
  BOOST_CHECK_EQUAL(ret.y, 15.0);
  BOOST_CHECK_EQUAL(ret.width, 109.0);
  BOOST_CHECK_EQUAL(ret.height, 55.0);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_tl,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_TL, {.x = 13, .y = 24});

  Rectangle ret = get_new_coordinates({.x = 6, .y = 12});
  BOOST_CHECK_EQUAL(ret.x, 3.0);
  BOOST_CHECK_EQUAL(ret.y, 3.0);
  BOOST_CHECK_EQUAL(ret.width, 107.0);
  BOOST_CHECK_EQUAL(ret.height, 62.0);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_tr,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_TR, {.x = 108, .y = 17});

  Rectangle ret = get_new_coordinates({.x = 116, .y = 22});
  BOOST_CHECK_EQUAL(ret.x, 10.0);
  BOOST_CHECK_EQUAL(ret.y, 20.0);
  BOOST_CHECK_EQUAL(ret.width, 108.0);
  BOOST_CHECK_EQUAL(ret.height, 45.0);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_b,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_B, {.x = 37, .y = 57});

  Rectangle ret = get_new_coordinates({.x = 43, .y = 66});
  BOOST_CHECK_EQUAL(ret.x, 10.0);
  BOOST_CHECK_EQUAL(ret.y, 15.0);
  BOOST_CHECK_EQUAL(ret.width, 100.0);
  BOOST_CHECK_EQUAL(ret.height, 59.0);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_l,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_L, {.x = 12, .y = 40});

  Rectangle ret = get_new_coordinates({.x = 8, .y = 47});
  BOOST_CHECK_EQUAL(ret.x, 6.0);
  BOOST_CHECK_EQUAL(ret.y, 15.0);
  BOOST_CHECK_EQUAL(ret.width, 104.0);
  BOOST_CHECK_EQUAL(ret.height, 50.0);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_t,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_T, {.x = 19, .y = 24});

  Rectangle ret = get_new_coordinates({.x = 21, .y = 29});
  BOOST_CHECK_EQUAL(ret.x, 10.0);
  BOOST_CHECK_EQUAL(ret.y, 20.0);
  BOOST_CHECK_EQUAL(ret.width, 100.0);
  BOOST_CHECK_EQUAL(ret.height, 45.0);
}


BOOST_AUTO_TEST_CASE(should_calculate_new_position_for_resize_r,
                     * boost::unit_test_framework::tolerance(0.00001))
{
  start_drag(DragMode::RESIZE_R, {.x = 108, .y = 50});

  Rectangle ret = get_new_coordinates({.x = 156, .y = 46});
  BOOST_CHECK_EQUAL(ret.x, 10.0);
  BOOST_CHECK_EQUAL(ret.y, 15.0);
  BOOST_CHECK_EQUAL(ret.width, 148.0);
  BOOST_CHECK_EQUAL(ret.height, 50.0);
}

BOOST_AUTO_TEST_SUITE_END()
