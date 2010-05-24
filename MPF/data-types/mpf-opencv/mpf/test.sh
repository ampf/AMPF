#!/bin/sh

export GST_PLUGIN_PATH=`pwd`/../../mpf-core/mpf/.libs:`pwd`/.libs

if `echo "$@" | grep -vq '!$'`;then
  fixup='!'
else
  fixup=''
fi

gst-launch-0.10 videotestsrc ! \
video/x-raw-yuv,width=640,height=480 ! ffmpegcolorspace ! toiplimage ! \
"$@" $fixup \
fromiplimage ! ffmpegcolorspace ! xvimagesink
