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
  DragMode get_drag_mode(Glib::RefPtr<SelectionRect>& rect)
  {
    return rect->drag_mode_;
  }
};
}
BOOST_FIXTURE_TEST_SUITE(SelectionRect, mdl::SelectionRectTestFixture);


BOOST_AUTO_TEST_CASE(starts_with_no_drag_mode)
{
  auto rect = mdl::SelectionRect::create(10, 15, 100, 50);

  BOOST_CHECK_EQUAL(get_drag_mode(rect), DragMode::NONE);
}

BOOST_AUTO_TEST_SUITE_END()
