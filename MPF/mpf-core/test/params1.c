/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-voidstar.h>

#define COMPONENT_NAME "params1"
#define COMPONENT_DESC "Test component checks parameter management"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpfcomponenttemplate.h>

static void component_class_init(MpfComponentClass *klass) {
  {
    g_assert(klass != NULL);
    g_assert(MPF_IS_COMPONENT_CLASS(klass));
  }

  mpf_voidstar_add_input("input");

  mpf_add_param_int("param1", "Param1", "Test Param1", 0, 10, 1);
}

static void component_init(MpfComponent *component) {
}

static MpfComponentProcessReturn component_process(MpfComponent *component) {
  void *dummydata;

  mpf_voidstar_unref(mpf_voidstar_pull("input"));

  return MPF_GOOD;
}
