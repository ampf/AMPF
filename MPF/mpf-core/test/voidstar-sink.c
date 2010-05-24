/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-voidstar.h>

#define COMPONENT_NAME "voidstar-sink"
#define COMPONENT_DESC "Test component sinks void*"
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

  {
    g_assert(klass->num_sink_pads == 1);
    g_assert(klass->num_src_pads == 0);
    g_assert(klass->padtemplates != NULL);
    g_assert(klass->padtemplates->next == NULL);
    g_assert(GST_IS_PAD_TEMPLATE(klass->padtemplates->data));
  }
}

static void component_init(MpfComponent *component) {
}

static MpfComponentProcessReturn component_process(MpfComponent *component) {
  void *dummydata;

  {
    g_assert(component != NULL);
    g_assert(MPF_IS_COMPONENT(component));
  }

  fprintf(stderr,"VOIDSTAR_SINK: pulling buffer\n");
  dummydata = mpf_voidstar_pull("input");

  {
    GstBuffer *dummybuf;

    fprintf(stderr,"dummydata ptr is %p\n",dummydata);
    g_assert(dummydata != NULL);

    dummybuf = mpf_component_find_buffer(NULL,dummydata);
    g_assert(dummybuf != NULL);
    g_assert(GST_IS_BUFFER(dummybuf));
  }

  mpf_voidstar_unref(dummydata);

  return MPF_GOOD;
}
