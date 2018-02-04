#include <string>

#include "NumericEntry.hpp"

using namespace mdl;


NumericEntry::NumericEntry()
{
}


void NumericEntry::set_value(int text)
{
  set_text(std::to_string(text));
}


int NumericEntry::get_value() const
{
  return std::stoi(get_text());
}

