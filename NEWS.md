## 2.3.0

* Added a function to shift the starting frames of a range of filters.

## 2.2.0

* It's now possible to display two frames (the current frame and the
  one just before that) at the same time.


## 2.1.0

* The automatic logo detection should be slightly faster.

* It's now possible to change a filter's starting frame.

* Added undo/redo functionality.

* Fixed crash when encoding was cancelled.


## 2.0.0

* New automatic logo detector that can find most logos in the video
  automatically.

* Fixed 'Next filter' not working when the frame displayed was before
  the first filter.


## 1.2.0

* New 'cut' filter to remove parts of the video.

* Added buttons and key shortcuts to move to the previous and next
  filters.

* Added option to scroll the frame to make the filter visible when
  moving between filters.

* It's possible to display the FFmpeg log after encoding finishes.

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
