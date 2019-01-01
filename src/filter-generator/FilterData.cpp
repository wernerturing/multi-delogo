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
#include <cstring>
#include <string>
#include <istream>
#include <ostream>

#include "Exceptions.hpp"
#include "IOUtils.hpp"
#include "FilterData.hpp"
#include "FilterList.hpp"

using namespace fg;


const std::string FilterData::HEADER_ = "MDLV1";


FilterData::FilterData()
  : jump_size_(500)
{
}


void FilterData::set_movie_file(const std::string& movie_file)
{
  movie_file_ = movie_file;
}

void FilterData::set_jump_size(int jump_size)
{
  jump_size_ = jump_size;
}


std::string FilterData::movie_file() const
{
  return movie_file_;
}


int FilterData::jump_size() const
{
  return jump_size_;
}


FilterList& FilterData::filter_list()
{
  return filter_list_;
}


bool FilterData::is_filter_data(std::istream& in)
{
  char header[HEADER_.size()];
  in.read(header, HEADER_.size());
  if (memcmp(header, HEADER_.c_str(), HEADER_.size()) != 0) {
    return false;
  }

  std::string rest_of_line;
  fg::getline(in, rest_of_line);
  return rest_of_line == "";
}


void FilterData::load(std::istream& in)
{
  if (!is_filter_data(in)) {
    throw InvalidFilterDataException();
  }

  fg::getline(in, movie_file_);

  std::string jump_size_str;
  fg::getline(in, jump_size_str);
  try {
    jump_size_ = std::stoi(jump_size_str);
  } catch (std::invalid_argument& e) {
    throw InvalidFilterDataException();
  }

  filter_list_.load(in);
}


void FilterData::save(std::ostream& out) const
{
  out << HEADER_ << '\n';
  out << movie_file_ << '\n';
  out << std::to_string(jump_size_) << '\n';

  filter_list_.save(out);
}

