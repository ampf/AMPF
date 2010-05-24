/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>

#define COMPONENT_NAME "cvcvtcolor"
#define COMPONENT_DESC "OpenCV Colorspace conversion"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  MpfIplImageFormatList allformats;

  allformats = mpf_iplimage_formatlist(CV_8UC1, CV_8UC3, CV_16UC1, CV_16UC3, CV_32FC1, CV_32FC3);
//  allformats = MPF_IPLIMAGE_FORMAT_ANY;

  mpf_iplimage_add_input("sink", allformats);
  mpf_iplimage_add_output("src", allformats);
}

component_init() {
}

component_process() {
  IplImage *in = mpf_iplimage_pull("sink");
  IplImage *out = mpf_iplimage_make_from_pad("src");

//  mpf_iplimage_push("src", out);

  // dummy...
  in = out;

  return MPF_GOOD;
}
