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
#include "filter-generator/FilterList.hpp"
#include "filter-generator/Filters.hpp"

#include "FilterListAdapter.hpp"

using namespace mdl;


FilterListAdapter::FilterListAdapter(fg::FilterList& filter_list, LogoFinderCallback& callback)
  : filter_list_(filter_list)
  , callback_(callback)
{
}


void FilterListAdapter::success(const mdl::LogoFinderResult& result)
{
  filter_list_.insert(result.start_frame + 1,
                      fg::filter_ptr(new fg::DelogoFilter(result.x, result.y, result.width, result.height)));

  callback_.success(result);
}


void FilterListAdapter::failure(int start_frame, int end_frame)
{
  filter_list_.insert(start_frame + 1, fg::filter_ptr(new fg::ReviewFilter()));

  callback_.failure(start_frame, end_frame);
}
