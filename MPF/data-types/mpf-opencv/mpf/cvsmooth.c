/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>

#define COMPONENT_NAME "cvsmooth"
#define COMPONENT_DESC "OpenCV Smooth image"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
  int cvtype;
  int param1, param2, param3, param4;

  MpfIplImageFormatList *curformats;
};

#include <mpf/mpf-componenttemplate.h>

#define BNS_FORMAT		mpf_iplimage_formatlist(CV_8UC1, CV_8SC1, CV_16UC1, CV_16SC1, CV_32FC1)
#define BLURGAUSS_FORMATS	mpf_iplimage_formatlist(CV_8UC1, CV_8UC3, CV_8SC1, CV_8SC3, CV_32FC1, CV_32FC3)
#define MEDIANBILAT_FORMATS	mpf_iplimage_formatlist(CV_8UC1, CV_8UC3, CV_8SC1, CV_8SC3)

component_class_init() {
  MpfIplImageFormatList allformats;

  allformats = mpf_iplimage_formatlist(BNS_FORMAT, BLURGAUSS_FORMATS, MEDIANBILAT_FORMATS);

  mpf_iplimage_add_input("input", allformats);
  mpf_iplimage_add_output("output", allformats);

/*
  mpf_add_param_int("param1", "Param1", "OpenCV's 'param1'",
  0, 32, 3);
  mpf_add_param_int("param2", "Param2", "OpenCV's 'param2'",
  0, 32, 3);
  mpf_add_param_int("param3", "Param3", "OpenCV's 'param3'",
  0, 32, 7);
  mpf_add_param_int("param4", "Param4", "OpenCV's 'param4'",
  0, 32, 0);
*/
}

component_init() {
//  fprintf(stderr,"in cvsmooth_init, mpf_private is %p\n",&mpf_private);
  mpf_private.cvtype = CV_GAUSSIAN;
  mpf_private.param1 = 5;
  mpf_private.param2 = 5;
  mpf_private.param3 = 15;
  mpf_private.param4 = 0;
}

component_process() {
  IplImage *src, *dst;

//  fprintf(stderr,"mpf_private is %p\n",&mpf_private);

  // Pull the source image
  src = mpf_iplimage_pull("input");

  // Get an output image based on the smooth type
//  if ((mpf_private.cvtype == CV_BLUR) || (mpf_private.cvtype == CV_GAUSSIAN)) {
  if (0) {
    // Attempt to make it writable, or get back a copy if it can't be
    fprintf(stderr,"** CVSMOOTH: trying to make writable image\n");
    dst = mpf_iplimage_make_writable(src);
  } else {
    // Construct a new output image of the same parameters
    fprintf(stderr,"** CVSMOOTH: allocating new image\n");
    dst = mpf_iplimage_make_from_img(src);
  }

/*
  fprintf(stderr,"parameters are %d: %d,%d,%d,%d\n",mpf_private.cvtype,
     mpf_private.param1, mpf_private.param2,
     mpf_private.param3, mpf_private.param4);
*/

  // Do the actual smoothing operation
  fprintf(stderr,"** CVSMOOTH: smoothing from %p to %p\n",src,dst);
  cvSmooth(src, dst, mpf_private.cvtype,
     mpf_private.param1, mpf_private.param2,
     mpf_private.param3, mpf_private.param4);

  // We're done with the source image, give up the reference
  mpf_iplimage_unref(src);

  // Send the output image down the pipe
  mpf_iplimage_push("output", dst);

  return MPF_GOOD;
}
