#include <memory>

#include <glibmm/refptr.h>
#include <gdkmm/pixbuf.h>

#include <opencv2/opencv.hpp>

#include "gui/common/Exceptions.hpp"

#include "OpenCVFrameProvider.hpp"

using namespace mdl::opencv;


OpenCVFrameProvider::OpenCVFrameProvider(std::unique_ptr<cv::VideoCapture> video)
  : FrameProvider()
{
  video_ = std::move(video);
}


Glib::RefPtr<Gdk::Pixbuf> OpenCVFrameProvider::get_frame(int frame_number)
{
  video_->set(CV_CAP_PROP_POS_FRAMES, frame_number);

  cv::Mat bgr_frame;
  bool success = video_->read(bgr_frame);
  if (!success) {
    throw mdl::FrameNotAvailableException();
  }

  cv::cvtColor(bgr_frame, frame_, CV_BGR2RGB);
  return Gdk::Pixbuf::create_from_data(frame_.data,
                                       Gdk::COLORSPACE_RGB,
                                       false, 8,
                                       frame_.cols, frame_.rows,
                                       frame_.step);
}
