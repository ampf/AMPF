#!/bin/sh
$prefix gst-launch$version $args videotestsrc ! video/x-raw-yuv,width=640,height=480 ! \
    mpf-identity sleep=1 ! ffmpegcolorspace ! \
    toiplimage ! iplidentity ! fromiplimage ! \
    ffmpegcolorspace ! ximagesink
