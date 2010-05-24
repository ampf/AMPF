/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>
#include <cv.h>

#define COMPONENT_NAME "cvsobel"
#define COMPONENT_DESC "OpenCV Sobel edge detect"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
  int param1, param2, param3, param4;
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_iplimage_add_input("sink", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_iplimage_add_output("src", MPF_IPLIMAGE_FORMAT_ANY);

/*
  mpf_register_parameter_int("param1", "Param1", "OpenCV's 'param1'",
  0, 32, 1);
  mpf_register_parameter_int("param2", "Param2", "OpenCV's 'param2'",
  0, 32, 1);
  mpf_register_parameter_int("param3", "Param3", "OpenCV's 'param3'",
  0, 32, 1);
  mpf_register_parameter_int("param4", "Param4", "OpenCV's 'param4'",
  0, 32, 1);
*/
}

component_init() {
}

component_process() {

  IplImage *src = mpf_iplimage_pull("sink");
  IplImage *dst = mpf_iplimage_make_from_img(src);

  cvSobel(src, dst, 3, 3, 5);

  mpf_iplimage_push("src", dst);

  return MPF_GOOD;
}
