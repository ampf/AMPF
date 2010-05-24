/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-voidstar.h>

#define COMPONENT_NAME "voidstar-src"
#define COMPONENT_DESC "Test component sources void*"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpfcomponenttemplate.h>

static void component_class_init(MpfComponentClass *klass) {
  mpf_voidstar_add_output("output");
}

static void component_init(MpfComponent *component) {
}

static MpfComponentProcessReturn component_process(MpfComponent *component) {
  void *dummydata;

  // Allocate 16 bytes just cause
  dummydata = g_malloc(16);

  fprintf(stderr,"voidstar-src: pushing dummy data\n");
  mpf_voidstar_push("output", dummydata);
}
