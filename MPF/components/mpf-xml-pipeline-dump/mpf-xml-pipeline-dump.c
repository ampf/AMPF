/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-xml-pipeline-dump"
#define COMPONENT_DESC "Dumps XML representation of the pipline, then passes data through unmodified"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
  // int field1;
  int buffer_count;
  int init;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
  mpf_voidstar_add_input_request("input");
  mpf_voidstar_add_output("output");
  mpf_add_param_string("location", "File output", "Write an output XML file", 0);
}

/* Component instance initialization and parameter handling. */
component_setup() {
}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

  if (!mpf_private.init) {
    MpfComponent *comp = mpf_component_get_curcomponent();
    char* location = mpf_param_get_string("location");
    if (location != 0 && strcmp("stdout", location)) {
      FILE* file;
      gst_xml_write_file(gst_element_get_parent(&comp->element), file = fopen (location, "w"));
      fclose(file);
    } else {
      gst_xml_write_file (gst_element_get_parent(&comp->element), stdout);
    }
    mpf_private.init = 1;
  }

  /* Print a message to show we are doing something */
  mpf_private.buffer_count++;
  // fprintf(stdout, "%s buffer %d\n", COMPONENT_NAME, mpf_private.buffer_count);

  /* If the input image is not being sent down the pipeline then release it. */
  // mpf_iplimage_unref(src);

  /* Push the results out to the next component in the pipeline. */
  if (mpf_voidstar_is_linked("input")) {
    mpf_voidstar_push("output", mpf_voidstar_pull("input"));
  }

  return MPF_GOOD;
}

