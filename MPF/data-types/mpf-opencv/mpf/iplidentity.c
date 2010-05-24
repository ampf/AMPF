/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>

#define COMPONENT_NAME "iplidentity"
#define COMPONENT_DESC "IplImage identity (passthrough)"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_iplimage_add_input("input", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_iplimage_add_output("output", MPF_IPLIMAGE_FORMAT_ANY);
}

component_init() {
}

component_process() {
  IplImage *src;

  src = mpf_iplimage_pull("input");
  // fprintf(stderr,"got image %p\n",src);
  src = mpf_iplimage_make_from_img(src);
  // fprintf(stderr,"got writable image %p\n",src);
  mpf_iplimage_push("output", src);

  return MPF_GOOD;
}
