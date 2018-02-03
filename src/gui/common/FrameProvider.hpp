#ifndef FRAME_PROVIDER_H
#define FRAME_PROVIDER_H

#include <string>

#include <glibmm/objectbase.h>
#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>


namespace mdl {
  class FrameProvider : public Glib::ObjectBase
  {
  public:
    virtual Glib::RefPtr<Gdk::Pixbuf> get_frame(int frame_number) = 0;
  };


  Glib::RefPtr<FrameProvider> create_frame_provider(const std::string& movie_filename);
}


#endif // FRAME_PROVIDER_H
