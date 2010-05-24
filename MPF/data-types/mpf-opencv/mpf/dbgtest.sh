#!/bin/sh

export GST_PLUGIN_PATH=`pwd`/../../mpf-core/mpf/.libs:`pwd`/.libs

gdb --args gst-launch-0.10 videotestsrc ! \
video/x-raw-yuv,width=640,height=480 ! ffmpegcolorspace ! toiplimage ! \
"$@" \
fromiplimage ! ffmpegcolorspace ! xvimagesink
