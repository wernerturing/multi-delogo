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
#include <memory>
#include <vector>

#include <gtkmm.h>

#include "filter-generator/Filters.hpp"
#include "filter-generator/FilterList.hpp"

#include "common/Exceptions.hpp"

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
    list.insert(1, fg::filter_ptr(new fg::DelogoFilter(0, 0, 0, 0)));
    list.insert(201, fg::filter_ptr(new fg::DrawboxFilter(2, 2, 2, 2)));
    list.insert(101, fg::filter_ptr(new fg::NullFilter()));

    model = mdl::FilterListModel::create(list);

    model->signal_row_inserted().connect(sigc::mem_fun(*this, &FilterModelFixture::test_inserted_or_changed_signal_callback));
    model->signal_row_changed().connect(sigc::mem_fun(*this, &FilterModelFixture::test_inserted_or_changed_signal_callback));
    model->signal_row_deleted().connect(sigc::mem_fun(*this, &FilterModelFixture::test_deleted_signal_callback));
  }


  void test_inserted_or_changed_signal_callback(const Gtk::TreeModel::Path& path, const Gtk::TreeModel::iterator& iter)
  {
    saved_iter = iter;
  }


  void test_deleted_signal_callback(const Gtk::TreeModel::Path& path)
  {
    deleted_paths.push_back(path);
  }


  fg::FilterList list;
  Glib::RefPtr<mdl::FilterListModel> model;

  Gtk::TreeModel::iterator saved_iter;
  std::vector<Gtk::TreeModel::Path> deleted_paths;
};
BOOST_FIXTURE_TEST_SUITE(filterlist_model, FilterModelFixture)


BOOST_AUTO_TEST_CASE(test_get_flags)
{
  BOOST_CHECK_EQUAL(model->get_flags(), Gtk::TREE_MODEL_LIST_ONLY);
}


BOOST_AUTO_TEST_CASE(test_get_n_columns)
{
  BOOST_CHECK_EQUAL(model->get_n_columns(), 3);
}


BOOST_AUTO_TEST_CASE(test_get_column_type)
{
  BOOST_CHECK_EQUAL(model->get_column_type(0), Glib::Value<int>::value_type());
  BOOST_CHECK_EQUAL(model->get_column_type(1), Glib::Value<fg::filter_ptr>::value_type());
  BOOST_CHECK_EQUAL(model->get_column_type(2), Glib::Value<Glib::ustring>::value_type());
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
  BOOST_CHECK_EQUAL(row[model->columns.start_frame], 201);
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


BOOST_AUTO_TEST_CASE(get_iter_should_fail_for_non_existing_rows)
{
  auto iter = model->get_iter("10");
  BOOST_CHECK(!iter);
}


BOOST_AUTO_TEST_CASE(get_iter_should_fail_for_empty_model)
{
  fg::FilterList empty_list;
  Glib::RefPtr<mdl::FilterListModel> empty_model = mdl::FilterListModel::create(empty_list);

  auto iter = empty_model->get_iter("0");
  BOOST_CHECK(!iter);
}


BOOST_AUTO_TEST_CASE(get_iter_should_fail_for_invalid_path)
{
  Gtk::TreeModel::Path path;
  path.push_back(0);
  path.push_back(2);
  auto iter = model->get_iter(path);
  BOOST_CHECK(!iter);
}


BOOST_AUTO_TEST_CASE(test_iteration)
{
  Gtk::TreeNodeChildren children = model->children();
  auto iter = children.begin();

  auto row0 = *iter;
  BOOST_CHECK_EQUAL(row0[model->columns.start_frame], 1);
  fg::filter_ptr filter0 = row0[model->columns.filter];
  BOOST_CHECK_EQUAL(filter0->type(), fg::FilterType::DELOGO);
  BOOST_CHECK_EQUAL(row0[model->columns.filter_name], "delogo");

  ++iter;
  auto row1 = *iter;
  BOOST_CHECK_EQUAL(row1[model->columns.start_frame], 101);
  fg::filter_ptr filter1 = row1[model->columns.filter];
  BOOST_CHECK_EQUAL(filter1->type(), fg::FilterType::NO_OP);
  BOOST_CHECK_EQUAL(row1[model->columns.filter_name], "none");

  ++iter;
  auto row2 = *iter;
  BOOST_CHECK_EQUAL(row2.get_value(model->columns.start_frame), 201);
  fg::filter_ptr filter2 = row2.get_value(model->columns.filter);
  BOOST_CHECK_EQUAL(filter2->type(), fg::FilterType::DRAWBOX);
  BOOST_CHECK_EQUAL(row2.get_value(model->columns.filter_name), "drawbox");

  ++iter;
  BOOST_CHECK_EQUAL(iter, children.end());
}


BOOST_AUTO_TEST_CASE(test_get_for_frame)
{
  auto iter = model->get_for_frame(150);

  BOOST_REQUIRE(iter);

  Gtk::TreeRow row = *iter;
  BOOST_CHECK_EQUAL(row[model->columns.start_frame], 101);
  fg::filter_ptr filter = row[model->columns.filter];
  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(test_get_for_frame_in_empty_model)
{
  fg::FilterList empty_list;
  Glib::RefPtr<mdl::FilterListModel> empty_model = mdl::FilterListModel::create(empty_list);

  auto iter = empty_model->get_for_frame(10);
  BOOST_CHECK(!iter);
}


BOOST_AUTO_TEST_CASE(test_get_by_start_frame)
{
  auto iter = model->get_by_start_frame(101);
  Gtk::TreeRow row = *iter;
  fg::filter_ptr filter = row[model->columns.filter];
  BOOST_CHECK_EQUAL(filter->type(), fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(test_get_by_start_frame_non_existing)
{
  auto iter = model->get_by_start_frame(105);
  BOOST_CHECK(!iter);
}


BOOST_AUTO_TEST_CASE(test_get_by_start_frame_in_empty_model)
{
  fg::FilterList empty_list;
  Glib::RefPtr<mdl::FilterListModel> empty_model = mdl::FilterListModel::create(empty_list);

  auto iter = empty_model->get_by_start_frame(10);
  BOOST_CHECK(!iter);
}


BOOST_AUTO_TEST_CASE(test_insert)
{
  auto iter_from_before_insert = model->children().begin();
  auto returned_iter = model->insert(151, fg::filter_ptr(new fg::DelogoFilter(10, 20, 30, 40)));
  auto iter_from_after_insert = model->children().begin();

  BOOST_CHECK_EQUAL(list.size(), 4);
  BOOST_CHECK_EQUAL(list.get_by_position(2)->first, 151);
  BOOST_CHECK_NE(iter_from_before_insert.get_stamp(),
                 iter_from_after_insert.get_stamp()); // iters become invalid

  auto inserted_row = *saved_iter;
  BOOST_CHECK_EQUAL(inserted_row[model->columns.start_frame], 151);

  auto inserted_row_from_returned_iter = *returned_iter;
  BOOST_CHECK_EQUAL(inserted_row_from_returned_iter[model->columns.start_frame], 151);
}


BOOST_AUTO_TEST_CASE(test_insert_for_row_that_already_exists)
{
  BOOST_CHECK_THROW(model->insert(101, fg::filter_ptr(new fg::DelogoFilter(10, 20, 30, 40))),
                    mdl::DuplicateRowException);

  BOOST_CHECK_EQUAL(list.get_by_start_frame(101)->second->type(), fg::FilterType::NO_OP);
}


BOOST_AUTO_TEST_CASE(test_delete_row)
{
  auto iter_before = model->children()[1];
  auto path_before = model->get_path(iter_before);
  model->remove(iter_before);
  auto iter_after = model->children().begin();

  BOOST_CHECK_EQUAL(list.size(), 2);
  BOOST_CHECK_EQUAL(list.get_by_position(0)->first, 1);
  BOOST_CHECK_EQUAL(list.get_by_position(1)->first, 201);

  BOOST_TEST(deleted_paths == std::vector<Gtk::TreeModel::Path>{path_before},
             boost::test_tools::per_element());
  BOOST_CHECK_NE(iter_before.get_stamp(),
                 iter_after.get_stamp()); // iters become invalid
}


BOOST_AUTO_TEST_CASE(should_change_start_frame)
{
  auto iter_before = model->children()[1];
  auto path_before = model->get_path(iter_before);

  auto row = *iter_before;
  row[model->columns.start_frame] = 301;
  auto iter_after = model->children()[1];

  BOOST_TEST(list.size() == 3);
  BOOST_TEST(list.get_by_start_frame(301)->second->type() == fg::FilterType::NO_OP);
  BOOST_TEST(iter_before.get_stamp() != iter_after.get_stamp()); // iters become invalid

  BOOST_TEST(deleted_paths == std::vector<Gtk::TreeModel::Path>{path_before},
             boost::test_tools::per_element());
  auto changed_row = *saved_iter;
  BOOST_TEST(changed_row[model->columns.start_frame] == 301);
}


BOOST_AUTO_TEST_CASE(should_change_start_frame_overwriting_existing_filter)
{
  auto iter_before = model->children()[1];
  auto path_before = model->get_path(iter_before);
  auto path_overwritten = model->get_path(model->children()[2]);

  auto row = *iter_before;
  row[model->columns.start_frame] = 201;
  auto iter_after = model->children()[1];

  BOOST_TEST(list.size() == 2);
  BOOST_TEST(list.get_by_start_frame(201)->second->type() == fg::FilterType::NO_OP);
  BOOST_TEST(iter_before.get_stamp() != iter_after.get_stamp()); // iters become invalid

  std::vector<Gtk::TreeModel::Path> expected_deleted{path_overwritten, path_before};
  BOOST_TEST(deleted_paths == expected_deleted,
             boost::test_tools::per_element());
  auto changed_row = *saved_iter;
  BOOST_TEST(changed_row[model->columns.start_frame] == 201);
}


BOOST_AUTO_TEST_CASE(should_not_allow_changing_name)
{
  auto iter = model->children()[1];
  auto row = *iter;

  BOOST_CHECK_THROW(row[model->columns.filter_name] = "a", std::invalid_argument);
}


BOOST_AUTO_TEST_CASE(should_allow_changing_the_filter)
{
  auto iter_before = model->children()[1];
  auto row = *iter_before;

  auto new_filter = fg::filter_ptr(new fg::DrawboxFilter(10, 10, 10, 10));
  row[model->columns.filter] = new_filter;
  auto iter_after = model->children()[1];

  BOOST_CHECK_EQUAL(list.size(), 3);
  BOOST_CHECK_EQUAL(list.get_by_start_frame(101)->second->type(), fg::FilterType::DRAWBOX);
  BOOST_CHECK_EQUAL(iter_before.get_stamp(),
                    iter_after.get_stamp()); // iters remain valid

  auto changed_row = *saved_iter;
  fg::filter_ptr changed_filter = changed_row[model->columns.filter];
  BOOST_CHECK_EQUAL(changed_filter, new_filter);
}


void test_start_frame_and_x(const Glib::RefPtr<mdl::FilterListModel>& model, int start_frame, int x)
{
  BOOST_TEST_CONTEXT("x = " << x) {
    auto iter = model->get_by_start_frame(start_frame);
    BOOST_REQUIRE(iter);
    fg::filter_ptr filter = (*iter)[model->columns.filter];
    std::shared_ptr<fg::DelogoFilter> delogo = std::dynamic_pointer_cast<fg::DelogoFilter>(filter);
    BOOST_CHECK_EQUAL(delogo->x(), x);
  }
}

BOOST_AUTO_TEST_CASE(should_shift_start_frames_backwards)
{
  fg::FilterList list;
  for (int i = 0; i <= 9; ++i) {
    list.insert(100*i + 1, fg::filter_ptr(new fg::DelogoFilter(i, i, i, i)));
  }
  Glib::RefPtr<mdl::FilterListModel> model = mdl::FilterListModel::create(list);

  std::pair<int, int> frames = model->shift_frames(300, 611, -1);

  BOOST_CHECK_EQUAL(frames.first, 301);
  BOOST_CHECK_EQUAL(frames.second, 601);

  BOOST_CHECK_EQUAL(model->children().size(), 10);
  for (int i = 0; i <= 2; ++i) {
    test_start_frame_and_x(model, 100*i + 1, i);
  }
  for (int i = 3; i <= 6; ++i) {
    test_start_frame_and_x(model, 100*i, i);
  }
  for (int i = 7; i <= 9; ++i) {
    test_start_frame_and_x(model, 100*i + 1, i);
  }
}

BOOST_AUTO_TEST_CASE(should_shift_start_frames_forwards)
{
  fg::FilterList list;
  for (int i = 0; i <= 9; ++i) {
    list.insert(100*i + 1, fg::filter_ptr(new fg::DelogoFilter(i, i, i, i)));
  }
  Glib::RefPtr<mdl::FilterListModel> model = mdl::FilterListModel::create(list);

  std::pair<int, int> frames = model->shift_frames(700, 1100, 1);

  BOOST_CHECK_EQUAL(frames.first, 701);
  BOOST_CHECK_EQUAL(frames.second, 901);

  BOOST_CHECK_EQUAL(model->children().size(), 10);
  for (int i = 0; i <= 6; ++i) {
    test_start_frame_and_x(model, 100*i + 1, i);
  }
  for (int i = 7; i <= 9; ++i) {
    test_start_frame_and_x(model, 100*i + 2, i);
  }
}

BOOST_AUTO_TEST_SUITE_END()
