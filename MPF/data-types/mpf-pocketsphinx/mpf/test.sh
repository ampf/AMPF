#!/bin/sh

export GST_PLUGIN_PATH=$GST_PLUGIN_PATH:`pwd`/.libs

gst-launch-0.10 gconfaudiosrc ! audioconvert ! audioresample ! vader name=vad \
auto-threshold=true ! mpfpocketsphinx name=asr $* ! utterance-dump
