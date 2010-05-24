/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <fcntl.h>
// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-test-pipe"
#define COMPONENT_DESC "Template component that passes data through unmodified"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
  // int field1;
  int buffer_count;
  FILE *readPipe;
  FILE *writePipe;
};

static void pipe_reader(void* data);
static gboolean pad_event_handler(GstPad* pad, GstEvent* event);

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
  mpf_voidstar_add_input("input");
  mpf_voidstar_add_output("output");

  /* Add an IplImage input */
  // mpf_iplimage_add_input("input_iplimage", MPF_IPLIMAGE_FORMAT_ANY);

  /* Add an RDF output */
  // mpf_rdf_add_output("output_rdf");

  /* Add parameters (name, nickname, description, min, max, default) */
  // mpf_add_param_int("object-count", "Object Count", "Number of objects",
  //   0, 32, 3);
}

/* Component instance initialization and parameter handling. */
component_setup() {
    // mpf_component_get_curcomponent()->flags = MPF_DEBUG;

    // Setup named pipes based on actual component name and process id, which will be
    // unique.
    gchar *name = gst_element_get_name(GST_ELEMENT(component));
    printf("%s\n", name);
    // Make fifos based on pad names.  TODD: automate.
    int mask = umask(0);
    mkdir("/tmp/appscio-mpf", S_IRWXU | S_IRWXG | S_IRWXO);
    umask(mask);
    gchar* src = tempnam("/tmp/appscio-mpf", "pipe-");
    gchar* sink = tempnam("/tmp/appscio-mpf", "pipe-");
    printf("src=%s\n", src);
    printf("sink=%s\n", sink);
    mkfifo(src, S_IRWXU);
    mkfifo(sink, S_IRWXU);
    // Fork the process using shell &
    gchar *process = g_strdup_printf("cat < %s > %s &", sink, src);
    printf("system process=%s\n", process);
    system(process);
    // Open the pipes.  Order is important, must match order of shovel to avoid deadlock.
    mpf_private.writePipe = fopen(sink, "w");
    mpf_private.readPipe = fopen(src, "r");

    printf("readPipe=%p, writePipe=%p\n", mpf_private.readPipe, mpf_private.writePipe);

    GstPad *input = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input");
    gst_pad_set_event_function(input, pad_event_handler);

    // Start a thread to consume the sink
    GstPad *outputpad = gst_element_get_pad(GST_ELEMENT(component), "output");
    gst_pad_start_task(outputpad, pipe_reader, component);

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

  /* If the input image is not being sent down the pipeline then release it. */
  // mpf_iplimage_unref(src);

  /* Push the results out to the next component in the pipeline. */
  void *in = mpf_voidstar_pull("input");
  int len = mpf_voidstar_get_length(in);

  printf("pull (%d): %s", len, in);
  len = fwrite(in, 1, len, mpf_private.writePipe);
  printf("wrote: %d\n", len);
  fflush(mpf_private.writePipe);

  return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

static void pipe_reader(void* data) {
    MpfComponent *component = (MpfComponent*)data;
    mpf_component_set_curcomponent(component);
    GstPad *outputpad = gst_element_get_pad(&component->element, "output");
    gchar buf[32];
    size_t len = fread(buf, 1, sizeof(buf)-1, mpf_private.readPipe);
    if (len <= 0) {
        printf("pipe_reader done.\n");
        gst_task_pause(GST_PAD_TASK(outputpad));
        gst_pad_push_event(outputpad, gst_event_new_eos());
        return;
    }
    buf[len] = 0;
    printf("pipe_reader data (%d): %s", len, buf);
    void* out = mpf_voidstar_stralloc(buf);
    GstBuffer *gbuf = mpf_voidstar_get_buffer(component, out);
    mpf_voidstar_set_length(out, len);
    mpf_voidstar_push("output", out);
    mpf_voidstar_send_outbuffers();

}

// Look for end of stream on input, use it to close the writePipe.
static gboolean
pad_event_handler(GstPad *pad, GstEvent *event)
{
  GstElement *element = gst_pad_get_parent_element(pad);
  MpfComponent *component = mpf_component_get_curcomponent();
  // If EOS, poke a message out of the events pad.
  if (event->type == GST_EVENT_EOS) {
    printf("EOS\n");
    fclose(mpf_private.writePipe);
  }

  return TRUE;
}
