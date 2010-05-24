#!/bin/sh

source runenv

gst-launch-0.10 \
v4l2src ! video/x-raw-yuv,width=320,height=240 ! \
ffmpegcolorspace ! toiplimage ! tee name=tee0 ! \
objmarkup name=objmarkup0 ! fromiplimage ! ffmpegcolorspace ! ximagesink \
tee0. ! cvhaar ! objcorrelate ! objmarkup0.objsink

#filesrc location=/home/omega/media/leno.flv ! decodebin ! \
