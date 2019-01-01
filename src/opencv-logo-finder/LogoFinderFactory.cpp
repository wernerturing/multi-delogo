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

#include "filter-generator/FilterData.hpp"

#include "gui/common/LogoFinder.hpp"
#include "OpenCVLogoFinder.hpp"

#include "FilterListAdapter.hpp"


std::shared_ptr<mdl::LogoFinder> mdl::create_logo_finder(fg::FilterData& filter_data, mdl::LogoFinderCallback& callback, bool verbose)
{
  mdl::FilterListAdapter* adapter = new mdl::FilterListAdapter(filter_data.filter_list(), callback);
  return std::shared_ptr<mdl::LogoFinder>(
    new mdl::opencv::OpenCVLogoFinder(filter_data.movie_file(), *adapter, verbose),
    [adapter](mdl::LogoFinder* logo_finder) {
      delete logo_finder;
      delete adapter;
    });
}
