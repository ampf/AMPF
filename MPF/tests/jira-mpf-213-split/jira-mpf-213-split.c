/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>
#include <mpf/mpf-rdf.h>
// #include <mpf/mpf-iplimage.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "jira-mpf-213-split"
#define COMPONENT_DESC "Template component that passes data through unmodified"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
    int buffer_count;
    int loglevel;
    MpfLogger logger;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
    mpf_voidstar_add_input("input");
    mpf_voidstar_add_output("output");
    mpf_rdf_add_output("metadata");

    mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
    mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);

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
    // Initialize logging.
    mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

    if (mpf_param_get_int("mpf-debug"))
        mpf_component_get_curcomponent()->flags = MPF_DEBUG;

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {
    /* Get the input */
    // IplImage *src = mpf_iplimage_pull("input_iplimage");

	void* input = mpf_voidstar_pull("input");

    /* Do any processing */
    GrdfGraph *graph = mpf_rdf_new();
    GrdfNode *node = grdf_node_anon_new(graph, "count");
    grdf_stmt_new_nui(graph, node, "buffer_count", mpf_private.buffer_count);

    /* Print a message to show we are doing something */
    mpf_private.buffer_count++;
    fprintf(stdout, "%s buffer %d\n", COMPONENT_NAME, mpf_private.buffer_count);

    /* If the input image is not being sent down the pipeline then release it. */
    // mpf_iplimage_unref(src);

    /* Push the results out to the next component in the pipeline. */
    mpf_voidstar_push("output", input);
    mpf_rdf_push("metadata", graph);

    return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

