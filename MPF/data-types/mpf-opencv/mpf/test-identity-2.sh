#!/bin/sh

sleep=${sleep:=0.02}

$prefix gst-launch$version $args filesrc location=$video ! decodebin ! \
    mpf-identity sleep=$sleep dump=none ! ffmpegcolorspace ! \
    toiplimage ! iplidentity ! fromiplimage ! \
    videoscale ! ffmpegcolorspace ! ximagesink max-lateness=-1

