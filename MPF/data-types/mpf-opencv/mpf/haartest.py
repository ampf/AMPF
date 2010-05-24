#!/usr/bin/env python

import time
import gobject
import pygst
pygst.require("0.10")
import gst

#gst.debug_set_default_threshold(5)

# Front-end of the pipeline
pipeline = gst.Pipeline("pipeline")
v4lsrc = gst.element_factory_make("v4lsrc", "v4lsrc")
ffmpegcolorspace0 = gst.element_factory_make("ffmpegcolorspace", "ffmpegcolorspace0")
toiplimage = gst.element_factory_make("toiplimage", "toiplimage")
tee = gst.element_factory_make("tee", "tee")

pipeline.add(v4lsrc,ffmpegcolorspace0,toiplimage,tee)

v4lsrc.link_filtered(ffmpegcolorspace0,
	gst.Caps("video/x-raw-yuv,width=320,height=240"))
gst.element_link_many(ffmpegcolorspace0,toiplimage,tee)


# Direct display
if 0:
  fromiplimage2 = gst.element_factory_make("fromiplimage", "fromiplimage2")
  ffmpegcolorspace2 = gst.element_factory_make("ffmpegcolorspace", "ffmpegcolorspace2")
  xvimagesink2 = gst.element_factory_make("xvimagesink", "xvimagesink2")

  pipeline.add(fromiplimage2,ffmpegcolorspace2,xvimagesink2)

  gst.element_link_many(tee,fromiplimage2,ffmpegcolorspace2,xvimagesink2)


# Haar testing
if 1:
#  cvhaar = gst.element_factory_make("cvhaar", "cvhaar")
#  pipeline.add(cvhaar)
#  gst.element_link_many(tee,cvhaar)

  objmarkup = gst.element_factory_make("objmarkup", "objmarkup")
  pipeline.add(objmarkup)
  tee.link(objmarkup)

#  cvhaar.link_pads("output",objmarkup,"objsink")

  fromiplimage1 = gst.element_factory_make("fromiplimage", "fromiplimage1")
  ffmpegcolorspace1 = gst.element_factory_make("ffmpegcolorspace", "ffmpegcolorspace1")
  xvimagesink1 = gst.element_factory_make("xvimagesink", "xvimagesink1")
  pipeline.add(fromiplimage1,ffmpegcolorspace1,xvimagesink1)
  gst.element_link_many(fromiplimage1,ffmpegcolorspace1,xvimagesink1)
  gst.element_link_many(objmarkup,fromiplimage1)



pipeline.set_state(gst.STATE_PLAYING)

while 1:
  time.sleep(1)

#gst-launch-0.10 --gst-debug=*:2 v4lsrc ! video/x-raw-yuv,width=320,height=240 ! 
#ffmpegcolorspace ! toiplimage ! tee ! cvhaar tee0. ! objmarkup ! fromiplimage ! 
#ffmpegcolorspace ! xvimagesink
