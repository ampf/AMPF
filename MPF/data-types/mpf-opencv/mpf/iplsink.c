/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>

#define COMPONENT_NAME "iplsink"
#define COMPONENT_DESC "IplImage sink"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_iplimage_add_input("input", MPF_IPLIMAGE_FORMAT_ANY);
}

component_init() {
}

component_process() {
  IplImage *src;

  src = mpf_iplimage_pull("input");
  fprintf(stderr,"got image %p\n",src);
  mpf_iplimage_unref(src);

  return MPF_GOOD;
}
