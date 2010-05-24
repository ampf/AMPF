/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
// #include <mpf/mpf-iplimage.h>
#include <mpf/mpf-rdf.h>
#include <mpf/mpf-logging.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-loi-smooth"
#define COMPONENT_DESC "Smooth loi metadata to avoid false segment detection"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	gint threshold;
	gint sustain;
	gint preset;
	gint loi;
	gint counter;
	gint setup;
	gint loglevel;
	MpfLogger logger;
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
    mpf_add_param_int("preset", "Preset", "Threshold for LOI to initiate segment", 0, INT_MAX, 0);
    mpf_add_param_int("threshold", "LOI Threshold", "Threshold for LOI to start sustain", 0, 100, 50);
	mpf_add_param_int("sustain", "LOI Sustain", "How many buffers long to sustain the output LOI", 0, INT_MAX, 10);

}

/* Component instance initialization and parameter handling. */
component_setup() {
	mpf_private.sustain = mpf_param_get_int("sustain");
	mpf_private.preset = mpf_param_get_int("preset");
	mpf_private.threshold = mpf_param_get_int("threshold");

    mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	GrdfGraph *graph = mpf_rdf_pull("input");

	if (graph->stmts != NULL) {

		GList *lois = grdf_graph_find_pred_uri(graph,
				"urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel");

		if (lois) {
			// Dig level of interest out of the predicate.
			GrdfStmt *stmt = GRDF_STMT(lois->data);
			GrdfNode *node = stmt->obj;
			gint loi = grdf_node_literal_get_int(node);

			MPF_PRIVATE_DEBUG("input loi=%-3d\n", loi);

			// Simple filtering algorithm: loi must exceed threshold for 'preset' consecutive
			// frames, then segment will begin, and will be sustained if threshold drops below
			// threshold for 'sustain' frames.
			if (loi >= mpf_private.threshold) {
				if (mpf_private.setup == mpf_private.preset) {
					mpf_private.counter = mpf_private.sustain+1;
					mpf_private.loi = loi;
				} else {
					mpf_private.setup++;
					MPF_PRIVATE_DEBUG("setup=%d\n", mpf_private.setup);
					if (mpf_private.setup == mpf_private.preset) {
						MPF_PRIVATE_DEBUG("starting segment\n");
					}
				}
			} else if (mpf_private.counter > 0) {
				mpf_private.counter--;
				// Reset the preset counter at the end of a segment.
				if (mpf_private.counter==0) {
					mpf_private.setup = 0;
					MPF_PRIVATE_DEBUG("ending segment.\n");
				}
			}
			loi = 0;
			if (mpf_private.counter > 0) {
				loi = mpf_private.loi;
			}
			grdf_node_literal_set_int(node, loi);

			MPF_PRIVATE_DEBUG("output loi=%-3d counter=%-3d\n", loi, mpf_private.counter);
		}

		// Push the results out to the next component in the pipeline.
		mpf_rdf_push("output", graph);

		return MPF_GOOD;
	}
}

/* Component instance finalization. */
component_teardown() {
}

