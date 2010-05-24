/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-rdf.h>
#include <mpf/mpf-logging.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-buf-rdf"
#define COMPONENT_DESC "Processes buffers and builds generic RDF descriptions of them.  Useful for test."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
    // int field1;
    int buffer_count;
    gchar *key;
    MpfLogger logger;
    int loglevel;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
    mpf_voidstar_add_input("input");
    mpf_rdf_add_output("rdf");

    mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
    mpf_add_param_string("key", "Key", "Key to use for rdf triples", 0);

}

/* Component instance initialization and parameter handling. */
component_setup() {
    // Initialize logging.
    mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));
    mpf_private.key = mpf_param_get_string("key");
    if (!mpf_private.key)  {
        mpf_private.key = gst_element_get_name(GST_ELEMENT(component));
    }
    // Force init of GRDF to avoid multi-threading problems.
    grdf_init();
}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {
    /* Do any processing */
    GrdfGraph *rdf = mpf_rdf_new();
    GrdfNode *node = grdf_node_anon_new(rdf, mpf_private.key);

    void* input = mpf_voidstar_pull("input");

    gchar *prefix = "urn:rdf:appscio.com/ver_1.0/buffer";

    gchar *key = g_strdup_printf("%s", prefix, mpf_private.key);
    gchar *bufferkey = g_strdup_printf("%s/index", prefix);
    gchar *lengthkey = g_strdup_printf("%s/length", prefix);
    gchar *timekey = g_strdup_printf("%s/timestamp", prefix);

    gchar *length = g_strdup_printf("%d", mpf_voidstar_get_length(input));
    gchar *count = g_strdup_printf("%d", mpf_private.buffer_count);
    gchar *time = g_strdup_printf("%lld", mpf_buffer_get_timestamp(input));

    grdf_stmt_new_nuu(rdf, node, "dcterms:type", prefix);
    grdf_stmt_new_nul(rdf, node, bufferkey, count);
    grdf_stmt_new_nul(rdf, node, lengthkey, length);
    grdf_stmt_new_nul(rdf, node, timekey, time);

    /* Print a message to show we are doing something */
    mpf_private.buffer_count++;
    gchar *name = gst_element_get_name(GST_ELEMENT(component));
    MPF_PRIVATE_DEBUG("%s buffer %d\n", name, mpf_private.buffer_count);

    /* Push the results out to the next component in the pipeline. */
    mpf_rdf_push("rdf", rdf);

    mpf_voidstar_unref(input);

    g_free(key);
    g_free(bufferkey);
    g_free(lengthkey);
    g_free(length);
    g_free(count);

    return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

