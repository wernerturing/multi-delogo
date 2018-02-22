# Using multi-delogo

This document will describe how to use multi-delogo to remove logos from videos.

We'll use a sample video, distributed with multi-delogo: [sample.mp4](../sample.mp4). This video shows and adorable kitten playing, but unfortunately it has some text added to it. Moreover, the text changes place several times during the video:

![Video frame](../images/video1.png)
![Video frame](../images/video2.png)

multi-delogo allows you to mark all the occurrences of the text, and generate a video removing the texts.


## Starting

When you run the program, you'll see a screen with two options:

* **Start a new project** starts a new project from a video file.
* **Open an existing project** allows you to continue a project you've already started.

Download the [sample video](../sample.mp4) if you haven't already, run multi-delogo and select **Start a new project**. Select the `sample.mp4` file.


## Navigating frames

This is the main window:

![multi-delogo main window](images/main-window.png)

It displays one frame of the video at a time.

To navigate a single frame backward or forward, use the < and > buttons, or press **s** or **d**, respectively.

To move a larger number of frames backward or forward, use the << and >> buttons, or the **a** and **d** keys, respectively. These buttons move by the number of frames specified in the **Jump size**, and you can change the jump size as needed.

You can also go directly to a frame by entering its number and pressing Enter.


## Defining a filter

In the example video, the text is not displayed in the first frames. We'll have to navigate to the first frame in which the text appears.

In a real case, you'd need to move forward the frames until the text is displayed, using the jump feature to avoid moving frames one by one.

In our example, the first frame with the text is frame 76. So move to frame 76.

Now click near the top left corner of the text, and drag the mouse, drawing a rectangle marking the are with the text:

![First filter added](images/first-filter.png)

Now look at the left side of the window. Notice that there is a row in the list saying **76 - delogo**. That is a _filter_.

A _filter_ is a modification done to the video, in order to remove the logos. Two types of filters are supported: [delogo](https://ffmpeg.org/ffmpeg-filters.html#delogo) attempts to remove the logo, and [drawbox](https://ffmpeg.org/ffmpeg-filters.html#drawbox) simply draws a black box over the text. Generally delogo works well, but depending on the video it might fail or generate ugly artifacts. The _none_ filter is used to not apply any filter to a part of the video.

Filters are applied from their _start frame_ until the start of the next filter, or until the end of the video if it's the last filter.


## Adding more filters

The next step is to move to the frame where the position of the text changes, and add a new filter starting at that frame. That means using the frame navigation buttons until that frame is found.

If the number of frames between logo changes is the same, the jump feature can be helpful: define the jump size to the number of frames each logo is displayed for, and the << and >> buttons will move between the frames where the logo changes. In our sample video, each logo is displayed for 75 frames, so define 75 as the jump size, and click the >> button. You should be at frame 151, and the logo at the top left is not displayed anymore. Instead, a new logo at the top right is now displayed (use the scroll bar or the zoom feature if that part of the image is not visible).

To define the new position for the logo, the process is the same: draw a rectangle covering it. You should end with a result like this:

![Second filter added](images/second-filter.png)

Note the new filter added to the list, starting at frame 151.

Now just repeat the process, marking each new position for the logo. In our short sample, there are just four positions:

![All filters added](images/all-filters.png)


## Editing filters

The list on the left displays all the filters set for the current project. Selecting a row moves to the start frame of that video, and displays the area in which the video is applied.

To edit an existing filter, you have to be on that filter's start frame. You can move the rectangle by dragging it, and resize it by dragging the bottom right corner. It's also possible to draw another rectangle, replacing the current one. Another possibility is to use the **x**, **y**, **width** and **height** controls below the filter list.

If you are not at a filter's start frame and make any change to the filter, a new filter is added.

To remove a filter, select it and press the button with the minus sign below the filter list.


## Encoding the video

Once all the filters are set up, it's time to generate a new video with the logos removed.

To do that, click the **Encode** button. A new window will be opened:

![Encode window](images/encode.png)

First, select the output file by using the **Select** button or typing a location.

### Format and quality

The options in **Video format** and **Quality** define the video codec to use and the quality of the resulting video.

**If in doubt, you can accept the defaults.** They should generate a high-quality video that is compatible with most players.

Two codecs are supported: H.264 and H.265. H.265 produces smaller files, but encoding is slower, and not all players support it. H.264 is more compatible and faster, but the files are larger.

For both codecs, CRF mode is used. The **Quality** number defines the video quality: in general, lower numbers generate better looking videos, but the file sizes are larger. If in doubt, use the defaults. For more details, see https://trac.ffmpeg.org/wiki/Encode/H.264#crf and https://trac.ffmpeg.org/wiki/Encode/H.265 .

### Running the encoder

To do the actual encoding, press the **Encode** button. This will start ffmpeg to encode the video, applying the filters. Encoding may take a long time and cannot be interrupted.

ffmpeg is included in the Windows download, but for Linux you'll have to install it. Your distribution probably includes a package for it.

### Running ffmpeg manually

If you want more control over the encoding process, you can run ffmpeg manually. To do that, instead of **Encode**, use **Generate filter script**. This generates a file with the description of the filters to apply, that can be passed to ffmpeg with the `-filter_script` option.
