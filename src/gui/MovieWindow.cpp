#include <gtkmm.h>

#include <opencv2/opencv.hpp>

#include "MovieWindow.hpp"

using namespace mdl;


MovieWindow::MovieWindow(const std::string& filename, int frameNumber)
  : Gtk::ApplicationWindow(), image_()
{
  cv::VideoCapture video(filename);
  if (!video.isOpened()) {
    printf("Cannot open the video file\n");
    return;
  }

  video.set(CV_CAP_PROP_POS_FRAMES, frameNumber);

  cv::Mat frame;
  bool success = video.read(frame);
  if (!success) {
    printf("Could not read frame\n");
    return;
  }

  // Leaks memory
  cv::Mat* frame2 = new cv::Mat();
  cv::cvtColor(frame, *frame2, CV_BGR2RGB);
  auto pixbuf = Gdk::Pixbuf::create_from_data(frame2->data,
                                              Gdk::COLORSPACE_RGB,
                                              false, 8,
                                              frame2->cols, frame2->rows,
                                              frame2->step);
  image_.set(pixbuf);

  add(image_);
}
