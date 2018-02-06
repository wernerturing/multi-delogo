#ifndef MDL_OPENCV_OPENCV_FRAME_PROVIDER_H
#define MDL_OPENCV_OPENCV_FRAME_PROVIDER_H

#include <string>
#include <memory>

#include <glibmm/objectbase.h>
#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

#include "gui/common/FrameProvider.hpp"


namespace mdl { namespace opencv {
  class OpenCVFrameProvider : public FrameProvider
  {
  public:
    OpenCVFrameProvider(std::unique_ptr<cv::VideoCapture> video);

    virtual Glib::RefPtr<Gdk::Pixbuf> get_frame(int frame_number);

  private:
    std::unique_ptr<cv::VideoCapture> video_;
    // We must keep a copy of the image data, since Gdk::Pixbuf does not make a copy of the data
    cv::Mat frame_;
  };
} }


#endif // MDL_OPENCV_OPENCV_FRAME_PROVIDER_H
