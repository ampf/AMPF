/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-rdf.h>
#include <mpf/mpf-logging.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-rdf-mux"
#define COMPONENT_DESC "Join two RDF streams into one, buffering latest."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	// int field1;
	int buffer_count;
	int eos_count;
	MpfLogger logger;
	int loglevel;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

static gboolean
pad_event_handler(GstPad *pad, GstEvent *event);

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
	mpf_rdf_add_output("output");
	mpf_rdf_add_input("input2");
	mpf_rdf_add_input("input1");
	mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);
	mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);

}

/* Component instance initialization and parameter handling. */
component_setup() {
	if (mpf_param_get_int("mpf-debug"))
		mpf_component_get_curcomponent()->flags = MPF_DEBUG;
	// Initialize logging.
	mpf_private.loglevel = mpf_param_get_int("loglevel");
	mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

	// Force init of GRDF to avoid multi-threading problems.
	grdf_init();
	// Establish EOS handler.
	GstPad *input1 = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input1");
	gst_pad_set_event_function(input1, pad_event_handler);
	GstPad *input2 = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input2");
	gst_pad_set_event_function(input2, pad_event_handler);

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	GrdfGraph *input1, *input2;

	input1 = mpf_rdf_pull("input1");
	input2 = mpf_rdf_pull("input2");

	GstBuffer *buf = mpf_rdf_get_buffer(component, input1);
	MPF_PRIVATE_DEBUG("timestamp=%lld\n", GST_BUFFER_TIMESTAMP(buf));

	/* Print a message to show we are doing something */
	gchar *name = gst_element_get_name(GST_ELEMENT(component));
	mpf_private.buffer_count++;
	MPF_PRIVATE_DEBUG("%s buffer %d\n", name, mpf_private.buffer_count);

	GrdfGraph *rdf = mpf_rdf_new();
	GrdfNode *node = grdf_node_anon_new(rdf, "mux");
	grdf_stmt_new_nuu(rdf, node, "dcterms:type", "urn:rdf:appscio.com/ver_1.0/mux");

	// mux the rdf.  TODO: synchronize time by buffering inputs.
	// Add a predicate to the input graph relating it to the mux output.
	if (grdf_graph_num_stmt(input1)) {
		gint nstmt = grdf_graph_num_stmt(input1);
		GrdfStmt *stmt = grdf_graph_get_stmt(input1, 0);
		GrdfStmt *muxer = grdf_stmt_new_nun(rdf, stmt->subj, "urn:rdf:appscio.com/ver_1.0/mux", node);
		int i;
		for (i=0; i<nstmt; i++) {
			grdf_graph_add_stmt(rdf, grdf_graph_get_stmt(input1, i));
		}
	}
	if (grdf_graph_num_stmt(input2)) {
		GrdfStmt *stmt = grdf_graph_get_stmt(input2, 0);
		GrdfStmt *muxer = grdf_stmt_new_nun(rdf, stmt->subj, "urn:rdf:appscio.com/ver_1.0/mux", node);
		gint nstmt = grdf_graph_num_stmt(input2);
		int i;
		for (i=0; i<nstmt; i++) {
			grdf_graph_add_stmt(rdf, grdf_graph_get_stmt(input2, i));
		}
	}
	/* Push the results out to the next component in the pipeline. */
	mpf_rdf_push("output", rdf);
	mpf_rdf_unref(input1);
	mpf_rdf_unref(input2);

	return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

// Event handling.
static gboolean
pad_event_handler(GstPad *pad, GstEvent *event)
{
	GstElement *element = gst_pad_get_parent_element(pad);
	MpfComponent *component = MPF_COMPONENT (GST_OBJECT_PARENT (pad));
	if (event->type == GST_EVENT_EOS) {
		mpf_private.eos_count++;
		MPF_LOG_DEBUG(&mpf_private.logger, "EOS %d\n", mpf_private.eos_count);
		if (mpf_private.eos_count == 2) {
			GstPad *output = gst_element_get_pad(GST_ELEMENT(component), "output");
			gst_pad_push_event(output, gst_event_new_eos());
		}
		return TRUE;
	}
	return gst_pad_event_default(pad, event);
}
