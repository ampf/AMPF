/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>
#include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-loi-stats"
#define COMPONENT_DESC "Generate loi statistics over a given interval."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	gint sum;
	gint max;
	gint min;
	gint count;
	float interval;
    int loglevel;
    MpfLogger logger;
    GstClockTime last;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
    mpf_rdf_add_input("input");
    mpf_rdf_add_output("output");

    mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
    mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);

    mpf_add_param_float("interval", "Interval", "Interval to compute stats for loi (seconds)", 0, FLT_MAX, 0);
}

/* Component instance initialization and parameter handling. */
component_setup() {
    // Initialize logging.
    mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

    mpf_private.interval = mpf_param_get_float("interval");

    if (mpf_param_get_int("mpf-debug"))
        mpf_component_get_curcomponent()->flags = MPF_DEBUG;

    mpf_private.min=INT_MAX;
    mpf_private.count=0;
}

#define NANOS 1000000000L

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	GrdfGraph *graph = mpf_rdf_pull("input");

	GstClockTime time = mpf_buffer_get_timestamp(graph);

	if (!mpf_private.last) {
		mpf_private.last = time;
	}

	GList *lois = grdf_graph_find_pred_uri(graph,
			"urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel");

	// Dig level of interest out of the predicate.
	GrdfStmt *stmt = GRDF_STMT(lois->data);
	GrdfNode *node = stmt->obj;
	if (graph->stmts != NULL) {

		gint loi = grdf_node_literal_get_int(node);
		mpf_private.sum += loi;
		if (loi > mpf_private.max)
			mpf_private.max = loi;
		if (loi < mpf_private.min)
			mpf_private.min = loi;
		mpf_private.count++;
	}


	if (mpf_private.interval > 0 && (float)(time - mpf_private.last)/NANOS >= mpf_private.interval) {
		grdf_stmt_new_nui(graph, stmt->subj, "urn:rdf:appscio.com/ver_1.0/loi/interval", mpf_private.interval);
		grdf_stmt_new_nui(graph, stmt->subj, "urn:rdf:appscio.com/ver_1.0/loi/count", mpf_private.count);
		grdf_stmt_new_nui(graph, stmt->subj, "urn:rdf:appscio.com/ver_1.0/loi/sum", mpf_private.sum);
		grdf_stmt_new_nui(graph, stmt->subj, "urn:rdf:appscio.com/ver_1.0/loi/min", mpf_private.min);
		grdf_stmt_new_nuf(graph, stmt->subj, "urn:rdf:appscio.com/ver_1.0/loi/mean", (double)mpf_private.sum/mpf_private.count);
		grdf_stmt_new_nui(graph, stmt->subj, "urn:rdf:appscio.com/ver_1.0/loi/max", mpf_private.max);
		mpf_rdf_push("output", graph);
		MPF_PRIVATE_INFO("sum=%d reset after interval=%f\n", mpf_private.sum, mpf_private.interval);
		mpf_private.last = time;
		mpf_private.sum = 0;
		mpf_private.max = 0;
		mpf_private.count = 0;
	}

    return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

