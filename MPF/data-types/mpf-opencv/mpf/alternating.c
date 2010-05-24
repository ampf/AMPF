/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>

#define COMPONENT_NAME "alternating"
#define COMPONENT_DESC "Alternating-image test"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
  gint phase;
};

#include <mpf/mpf-componenttemplate.h>

static void component_class_init(MpfComponentClass *klass) {
  mpf_iplimage_add_input("sink0", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_iplimage_add_input("sink1", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_iplimage_add_output("src", MPF_IPLIMAGE_FORMAT_ANY);
}

static void component_init(MpfComponent *component) {
  component->flags = 0xffffffff;
}

static MpfComponentProcessReturn component_process(MpfComponent *component) {
  struct component_private *priv = &MPF_COMPONENT_PRIVATE(component);
  int i;

  IplImage *src0 = mpf_iplimage_pull("sink0");
  IplImage *src1 = mpf_iplimage_pull("sink1");

  if (mpf_private.phase) {
    mpf_iplimage_push("src", src0);
    mpf_private.phase = 0;
  } else {
    mpf_iplimage_push("src", src1);
    mpf_private.phase = 1;
  }

  return MPF_GOOD;
}
