// g++ -o gtkshowframe `pkg-config gtkmm-3.0 opencv --cflags --libs` gtkshowframe.cpp

#include <gtkmm.h>

#include <opencv2/opencv.hpp>


int main(int argc, char *argv[])
{
  Gtk::Main gtk(argc, argv);

  if (argc != 3) {
    printf("usage: gtkshowframe <Video_Path> <Frame_Number>\n");
    return -1;
  }

  cv::VideoCapture video(argv[1]);
  if (!video.isOpened()) {
    printf("Cannot open the video file\n");
    return -1;
  }

  int frameNumber = atoi(argv[2]);
  video.set(CV_CAP_PROP_POS_FRAMES, frameNumber);

  cv::Mat frame;
  bool success = video.read(frame);
  if (!success) {
    printf("Could not read frame\n");
    return -1;
  }

  cv::cvtColor(frame, frame, CV_BGR2RGB);
  auto pixbuf = Gdk::Pixbuf::create_from_data(frame.data,
                                              Gdk::COLORSPACE_RGB,
                                              false, 8,
                                              frame.cols, frame.rows,
                                              frame.step);

  Gtk::Window window;
  window.set_default_size(600, 600);

  Gtk::Image image(pixbuf);
  window.add(image);

  window.show_all();
  gtk.run(window);
}
