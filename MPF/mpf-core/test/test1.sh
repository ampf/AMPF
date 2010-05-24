#!/bin/sh

export GST_PLUGIN_PATH=`pwd`/.libs

gst-launch fakesrc sizetype=fixed sizemax=4 ! voidstar-sink $*
