/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <string.h>
// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-test-mux"
#define COMPONENT_DESC "Mux input buffers by simple concatenation."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
  // int field1;
  int buffer_count;
  gboolean end_1;
  gboolean end_2;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

static gboolean pad_event_handler_1(GstPad* pad, GstEvent* event);
static gboolean pad_event_handler_2(GstPad* pad, GstEvent* event);

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
  mpf_voidstar_add_output("output");
  mpf_voidstar_add_input("input2")
  mpf_voidstar_add_input("input1");

  mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);
}

/* Component instance initialization and parameter handling. */
component_setup() {
    if (mpf_param_get_int("mpf-debug"))
      mpf_component_get_curcomponent()->flags = MPF_DEBUG;
    GstPad *input1 = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input1");
    gst_pad_set_event_function(input1, pad_event_handler_1);
    GstPad *input2 = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input2");
    gst_pad_set_event_function(input2, pad_event_handler_2);

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {
  /* Get the input */
  // IplImage *src = mpf_iplimage_pull("input_iplimage");

  /* Do any processing */
  // int count = mpf_param_get_int("object-count");
  // GrdfGraph *graph = mpf_rdf_new();
  // GrdfNode *node = grdf_node_anon_new(graph, "count");
  // grdf_stmt_new_nuu(graph, node, "isA", "AppscioScalar");
  // grdf_stmt_new_nui(graph, node, "AppscioScalarValue", count);

  /* Print a message to show we are doing something */
  mpf_private.buffer_count++;
  fprintf(stdout, "%s buffer %d\n", COMPONENT_NAME, mpf_private.buffer_count);

  void* input1 = mpf_voidstar_pull("input1");
  void* input2 = mpf_voidstar_pull("input2");

  /* Push the results out to the next component in the pipeline. */
  mpf_voidstar_ref(input1);

  int len1 = mpf_voidstar_get_length(input1);
  int len2 = mpf_voidstar_get_length(input2);
  void* output = mpf_voidstar_alloc(len1 + len2);
  memcpy(output, input1, len1);
  memcpy(output+len1, input2, len2);

  mpf_voidstar_push("output", output);
  mpf_voidstar_unref(input1);
  mpf_voidstar_unref(input2);

  return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

static gboolean
pad_event_handler_1(GstPad *pad, GstEvent *event)
{
  GstElement *element = gst_pad_get_parent_element(pad);
  MpfComponent *component = MPF_COMPONENT (GST_OBJECT_PARENT (pad));
  if (event->type == GST_EVENT_EOS) {
      printf("eos_input1\n");
      mpf_private.end_1 = TRUE;
      if (mpf_private.end_1 && mpf_private.end_2) {
          GstPad *output = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "output");
          gst_pad_push_event(output, gst_event_new_eos());
      }
      return TRUE;
  }
  return gst_pad_event_default(pad, event);
}

static gboolean
pad_event_handler_2(GstPad *pad, GstEvent *event)
{
  GstElement *element = gst_pad_get_parent_element(pad);
  MpfComponent *component = MPF_COMPONENT (GST_OBJECT_PARENT (pad));
  if (event->type == GST_EVENT_EOS) {
      printf("eos_input2\n");
      mpf_private.end_2 = TRUE;
      if (mpf_private.end_1 && mpf_private.end_2) {
          GstPad *output = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "output");
          gst_pad_push_event(output, gst_event_new_eos());
      }
      return TRUE;
  }
  return gst_pad_event_default(pad, event);
}
