#ifndef NUMERIC_ENTRY_H
#define NUMERIC_ENTRY_H

#include <gtkmm.h>

namespace mdl {
  class NumericEntry : public Gtk::Entry
  {
  public:
    NumericEntry();

    void set_value(int text);
    int get_value() const;
  };
}

#endif // NUMERIC_ENTRY_H
