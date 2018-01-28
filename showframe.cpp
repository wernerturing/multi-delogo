#include <stdio.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main(int argc, char** argv )
{
  if (argc != 3) {
    printf("usage: showframe <Video_Path> <Frame_Number>\n");
    return -1;
  }

  VideoCapture video(argv[1]);
  if (!video.isOpened()) {
    printf("Cannot open the video file\n");
    return -1;
  }

  int frameNumber = atoi(argv[2]);
  video.set(CV_CAP_PROP_POS_FRAMES, frameNumber);

  Mat frame;
  bool success = video.read(frame);
  if (!success) {
    printf("Could not read frame\n");
    return -1;
  }

  namedWindow("Video Frame", WINDOW_AUTOSIZE);
  imshow("Video Frame", frame);

  waitKey(0);

  return 0;
}
