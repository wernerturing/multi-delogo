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

  protected:
    virtual void on_insert_text(const Glib::ustring& text, int* position);

  private:
    bool contains_only_numbers(const Glib::ustring& text) const;
  };
}

#endif // NUMERIC_ENTRY_H
