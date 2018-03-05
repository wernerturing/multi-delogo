## [ongoing]

* Fixed bug that caused FFmpeg to fail if the last filter is a "none".


## 1.1.0

* Added 'zoom to fit' feature.

* Added a progress bar with estimated time remaining to the encoding
  window.

* If you try to open a video and there is already a project for that
  video, you'll be given the option to open the project instead.

* Added function to randomly increase filter times.

* Clicking on the image does not affect the selection rectangle.

* It's now possible to resize the selection rectangle using the other
  corners and borders, not only the bottom right corner.

* The temporary file with FFmpeg filters is properly deleted if FFmpeg
  fails to start.
