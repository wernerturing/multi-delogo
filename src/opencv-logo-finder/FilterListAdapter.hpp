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
#ifndef MDL_FILTER_LIST_ADAPATER_H
#define MDL_FILTER_LIST_ADAPATER_H

#include <memory>

#include "filter-generator/FilterData.hpp"
#include "filter-generator/FilterList.hpp"

#include "gui/common/LogoFinder.hpp"


namespace mdl {
  class FilterListAdapter : public mdl::LogoFinderCallback
  {
  public:
    FilterListAdapter(fg::FilterList& filter_list, LogoFinderCallback& callback);
    void success(const mdl::LogoFinderResult& result) override;
    void failure(int start_frame, int end_frame) override;

  private:
    fg::FilterList& filter_list_;
    LogoFinderCallback& callback_;
  };


  std::shared_ptr<LogoFinder> create_logo_finder(fg::FilterData& filter_data, LogoFinderCallback& callback, bool verbose);
}


#endif // MDL_FILTER_LIST_ADAPATER_H
