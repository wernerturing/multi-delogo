#include <string>
#include <memory>

#include <glibmm/refptr.h>

#include <opencv2/opencv.hpp>

#include "gui/common/FrameProvider.hpp"
#include "OpenCVFrameProvider.hpp"


Glib::RefPtr<mdl::FrameProvider> mdl::create_frame_provider(const std::string& movie_filename)
{
  std::unique_ptr<cv::VideoCapture> video(new cv::VideoCapture(movie_filename));
  // TODO: Check if (!video.isOpened())

  return Glib::RefPtr<mdl::FrameProvider>(new mdl::opencv::OpenCVFrameProvider(std::move(video)));
}
