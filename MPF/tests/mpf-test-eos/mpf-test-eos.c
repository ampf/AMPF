/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * SECTION: mpf-mpf-test-eos
 * @short_description: This component illustrates how to process data on an input pad
 * until an EOS is received, and then how to generate and send data on an output (events) pad.
 * This is useful in batch-like processing algorithms where downstream proocessing cannot
 * be performed until the input stream is finished (e.g. audio level compensation).
 */

/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>

// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-test-eos"
#define COMPONENT_DESC "Demonstrate processing triggered by EOS on an input pad."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"
#define COMPONENT_TYPE "Filter/Mpf/Test"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	int buffer_count;
	int loglevel;
	MpfLogger logger;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

static gboolean pad_event_handler(GstPad* pad, GstEvent* event);

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
	mpf_voidstar_add_input("input");
	mpf_voidstar_add_output("events");
	mpf_voidstar_add_output("output");

	mpf_add_param_int("loglevel", "Log Level",
			"Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG,
			MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
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
	mpf_private.loglevel = mpf_param_get_int("loglevel");
	mpf_logger_init(&mpf_private.logger, mpf_private.loglevel,
			gst_element_get_name(GST_ELEMENT(component)));
	GstPad *input = gst_element_get_pad(GST_ELEMENT(
			mpf_component_get_curcomponent()), "input");
	gst_pad_set_event_function(input, pad_event_handler);
	if (mpf_param_get_int("mpf-debug"))
		mpf_component_get_curcomponent()->flags = MPF_DEBUG;
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

	/* If the input image is not being sent down the pipeline then release it. */
	// mpf_iplimage_unref(src);

	/* Push the results out to the next component in the pipeline. */
	mpf_voidstar_pull("input");
	GString *string = g_string_new("");
	g_string_printf(string, "output.%d\n", mpf_private.buffer_count);
	mpf_voidstar_push("output", mpf_voidstar_stralloc(string->str));

	// g_string_printf(string, "no-event.%d\n", mpf_private.buffer_count);
	// mpf_voidstar_push("events", mpf_voidstar_stralloc(string->str));

	g_string_free(string, TRUE);
	// mpf_rdf_push("output_rdf", graph);

	return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

static gboolean pad_event_handler(GstPad *pad, GstEvent *event) {
	// Establish thread-local.
	MpfComponent *component = MPF_COMPONENT(GST_OBJECT_PARENT(pad));
	mpf_component_set_curcomponent(component);
	GstElement *element = gst_pad_get_parent_element(pad);

	gchar *elementname = gst_element_get_name(element);
	gchar *padname = gst_pad_get_name(pad);
	const gchar *eventname = gst_event_type_get_name(event->type);

	MPF_PRIVATE_ALWAYS("element=%s pad=%s event=%s\n", elementname, padname,
			eventname);
	// If EOS, poke a message out of the events pad.
	if (event->type == GST_EVENT_EOS) {
		GstPad *events = gst_element_get_pad(element, "events");

		printf("GstPad *events=%p\n", events);

		GString *string = g_string_new("");
		g_string_printf(string, "%s: EOS buffer_count=%d\n", elementname,
				mpf_private.buffer_count);
		mpf_voidstar_push("events", mpf_voidstar_stralloc(string->str));
		mpf_voidstar_send_outbuffers();
		gst_pad_push_event(events, gst_event_new_eos());
	}

	g_free(elementname);
	g_free(padname);
	return gst_pad_event_default(pad, event);
}
