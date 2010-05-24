/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>
#include <cv.h>

#define COMPONENT_NAME "cvcanny"
#define COMPONENT_DESC "OpenCV Canny edge detection"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
  int param1, param2, param3, param4;
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  MpfIplImageFormatList formats = mpf_iplimage_formatlist(CV_8UC1);

  mpf_iplimage_add_input("sink", formats);
  mpf_iplimage_add_output("src", formats);
}

component_init() {
}

component_process() {
  IplImage *in = mpf_iplimage_pull("sink");

  IplImage *out = cvCreateImage( cvGetSize(in), 8, 1 );
  cvCanny(in, out, 50, 200, 3 );
  mpf_iplimage_push("src", out);

  return MPF_GOOD;
}
