/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>

#include <gst/video/video.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "voidraw"
#define COMPONENT_DESC "Template component that passes data through unmodified"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
  /* int field1; */
  int buffer_count;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
  mpf_voidstar_add_input_with_caps("input",GST_VIDEO_CAPS_RGB);
//  mpf_voidstar_add_input("input");
  mpf_voidstar_add_output("output");
}

/* Component instance initialization and parameter handling. */
component_init() {
}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {
  void *frame;

  frame = mpf_voidstar_pull("input");

  fprintf(stderr,"mime/type is '%s'\n",mpf_buffer_get_mimetype(frame));
  fprintf(stderr,"frame is %dx%d\n",mpf_buffer_get_int(frame,"width"),mpf_buffer_get_int(frame,"height"));

  mpf_voidstar_push("output", frame);

  return MPF_GOOD;
}

