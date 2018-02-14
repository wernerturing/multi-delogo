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


class FilterModelFixture
{
public:
  FilterModelFixture()
  {
    list.insert(0, new fg::DelogoFilter(0, 0, 0, 0));
    list.insert(200, new fg::DrawboxFilter(2, 2, 2, 2));
    list.insert(100, new fg::NullFilter());

    model = mdl::FilterListModel::create(list);
  }


  void test_inserted_signal_callback(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
  {
    saved_iter = iter;
  }


  void test_deleted_signal_callback(const Gtk::TreeModel::Path& path)
  {
    saved_path = path;
  }


  fg::FilterList list;
  Glib::RefPtr<mdl::FilterListModel> model;

  Gtk::TreeModel::iterator saved_iter;
  Gtk::TreeModel::Path saved_path;
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
  Gtk::TreeNodeChildren root_children = model->children();
  BOOST_CHECK_EQUAL(root_children.size(), 3);

  Gtk::TreeRow first_row = root_children[0];
  BOOST_CHECK_EQUAL(first_row.children().size(), 0);
}


BOOST_AUTO_TEST_CASE(test_get_nth_child)
{
  Gtk::TreeNodeChildren root_children = model->children();
  Gtk::TreeRow row = root_children[2];
  BOOST_CHECK_EQUAL(row[model->columns.start_frame], 200);
}


BOOST_AUTO_TEST_CASE(test_get_parent)
{
  Gtk::TreeRow row = model->children()[2];
  Gtk::TreeIter iter_parent = row.parent();
  BOOST_CHECK(!iter_parent);
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
  Gtk::TreeNodeChildren children = model->children();
  auto iter = children.begin();

  auto row0 = *iter;
  BOOST_CHECK_EQUAL(row0[model->columns.start_frame], 0);
  fg::Filter* filter0 = row0[model->columns.filter];
  BOOST_CHECK_EQUAL(filter0->type(), fg::FilterType::DELOGO);

  ++iter;
  auto row1 = *iter;
  BOOST_CHECK_EQUAL(row1[model->columns.start_frame], 100);
  fg::Filter* filter1 = row1[model->columns.filter];
  BOOST_CHECK_EQUAL(filter1->type(), fg::FilterType::NO_OP);

  ++iter;
  auto row2 = *iter;
  BOOST_CHECK_EQUAL(row2.get_value(model->columns.start_frame), 200);
  fg::Filter* filter2 = row2.get_value(model->columns.filter);
  BOOST_CHECK_EQUAL(filter2->type(), fg::FilterType::DRAWBOX);

  ++iter;
  BOOST_CHECK_EQUAL(iter, children.end());
}


BOOST_AUTO_TEST_CASE(test_insert)
{
  model->signal_row_inserted().connect(sigc::mem_fun(*this, &FilterModelFixture::test_inserted_signal_callback));

  auto iter_from_before_insert = model->children().begin();
  model->insert(150, new fg::DelogoFilter(10, 20, 30, 40));
  auto iter_from_after_insert = model->children().begin();

  BOOST_CHECK_EQUAL(list.size(), 4);
  BOOST_CHECK_EQUAL(list.get_by_position(2)->first, 150);
  BOOST_CHECK_NE(iter_from_before_insert.get_stamp(),
                 iter_from_after_insert.get_stamp()); // iters become invalid

  auto inserted_row = *saved_iter;
  BOOST_CHECK_EQUAL(inserted_row[model->columns.start_frame], 150);
}


BOOST_AUTO_TEST_CASE(test_insert_for_row_that_already_exists)
{
  BOOST_CHECK_THROW(model->insert(100, new fg::DelogoFilter(10, 20, 30, 40)),
                    mdl::DuplicateRowException);

  BOOST_CHECK_EQUAL(list.get_by_start_frame(100)->second->type(), fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(test_delete_row)
{
  model->signal_row_deleted().connect(sigc::mem_fun(*this, &FilterModelFixture::test_deleted_signal_callback));

  auto iter_before = model->children()[1];
  auto path_before = model->get_path(iter_before);
  model->remove(iter_before);
  auto iter_after = model->children().begin();

  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list.get_by_position(0)->first, 0);
  BOOST_CHECK_EQUAL(list.get_by_position(1)->first, 200);

  BOOST_CHECK_EQUAL(saved_path, path_before);
  BOOST_CHECK_NE(iter_before.get_stamp(),
                 iter_after.get_stamp()); // iters become invalid
}

BOOST_AUTO_TEST_SUITE_END()
