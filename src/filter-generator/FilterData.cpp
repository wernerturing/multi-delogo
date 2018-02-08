#include <string>
#include <istream>
#include <ostream>

#include "Exceptions.hpp"
#include "FilterData.hpp"
#include "FilterList.hpp"

using namespace fg;


const std::string FilterData::HEADER_ = "MDLV1";


FilterData::FilterData()
{
}


FilterData::FilterData(const std::string& movie_file)
  : movie_file_(movie_file)
  , jump_size_(500)
{
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


void FilterData::load(std::istream& in)
{
  std::string header;
  std::getline(in, header);
  if (header != HEADER_) {
    throw InvalidFilterDataException();
  }

  std::getline(in, movie_file_);

  std::string jump_size_str;
  std::getline(in, jump_size_str);
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

