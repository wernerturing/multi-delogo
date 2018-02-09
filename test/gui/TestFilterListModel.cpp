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
#include "filter-generator/FilterList.hpp"

#include "FilterListModel.hpp"


#define BOOST_TEST_MODULE filterlist model
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


class FilterModelFixture
{
public:
  FilterModelFixture()
  {
    fg::FilterList list;
    list.insert(0, new fg::DelogoFilter(0, 0, 0, 0));
    list.insert(200, new fg::DrawboxFilter(2, 2, 2, 2));
    list.insert(100, new fg::NullFilter());

    model = mdl::FilterListModel::create(list);
  }

  Glib::RefPtr<mdl::FilterListModel> model;
};
BOOST_FIXTURE_TEST_SUITE(filterlist_model, FilterModelFixture)


BOOST_AUTO_TEST_CASE(test_get_flags)
{
  BOOST_CHECK_EQUAL(model->get_flags(), Gtk::TREE_MODEL_LIST_ONLY);
}


BOOST_AUTO_TEST_CASE(test_get_n_columns)
{
  BOOST_CHECK_EQUAL(model->get_n_columns(), 2);
}


BOOST_AUTO_TEST_CASE(test_get_column_type)
{
  BOOST_CHECK_EQUAL(model->get_column_type(0), Glib::Value<int>::value_type());
  BOOST_CHECK_EQUAL(model->get_column_type(1), Glib::Value<fg::Filter*>::value_type());
}


BOOST_AUTO_TEST_CASE(test_get_childen)
{
  auto children = model->children();
  BOOST_CHECK_EQUAL(children.size(), 3);
}


BOOST_AUTO_TEST_CASE(test_conversion_iter_and_path)
{
  auto iter = model->get_iter("1");
  auto path = model->get_path(iter);
  BOOST_CHECK_EQUAL(path.to_string(), "1");

  Gtk::TreeModel::Path path2_in;
  path2_in.push_back(2);
  auto iter2 = model->get_iter(path2_in);
  auto path2_out = model->get_path(iter2);
  BOOST_CHECK_EQUAL(path2_out, path2_in);
}


BOOST_AUTO_TEST_CASE(test_iteration)
{
  auto children = model->children();
  auto iter = children.begin();
  ++iter;
  ++iter;
  ++iter;
  BOOST_CHECK_EQUAL(iter, children.end());
}

BOOST_AUTO_TEST_SUITE_END()
