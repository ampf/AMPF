/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>
#include <mpf/mpf-rdf.h>
// #include <mpf/mpf-iplimage.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-loi-seg"
#define COMPONENT_DESC "Converts level-of-interest into segment metadata."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
    // int field1;
    int loglevel;
    MpfLogger logger;
	int threshold;
	long input_frame;
	int segment_start, segment_end;
	int segment_best_frame, segment_best_level;
	gboolean in_segment;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

static gboolean pad_event_handler(GstPad *pad, GstEvent *event) {

	// Establish thread-local.
	MpfComponent *component = MPF_COMPONENT(GST_OBJECT_PARENT(pad));
	mpf_component_set_curcomponent(component);
	GstElement *element = gst_pad_get_parent_element(pad);

    MPF_PRIVATE_INFO("pad_event_handler: %s\n", gst_event_type_get_name(
            event->type));

    if (event->type == GST_EVENT_EOS) {
		if (mpf_private.in_segment) {
			mpf_private.segment_end = mpf_private.input_frame-1;
			GrdfGraph *segment = mpf_rdf_new();
			GrdfNode *node = grdf_node_anon_new(segment, "segment");
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/start_frame", mpf_private.segment_start);
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/end_frame", mpf_private.segment_end);
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/best_frame", mpf_private.segment_best_frame);
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/best_interest", mpf_private.segment_best_level);
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/total_frames", mpf_private.input_frame);

			mpf_rdf_push("segments", segment);
			mpf_voidstar_send_outbuffers();

			MPF_PRIVATE_INFO("segments: start_frame=%d, end_frame=%d, frames=%d, best_frame=%d, best_interest=%d\n",
				mpf_private.segment_start, mpf_private.segment_end, mpf_private.segment_end-mpf_private.segment_start+1, mpf_private.segment_best_frame, mpf_private.segment_best_level);
			mpf_private.in_segment = FALSE;
			mpf_private.segment_best_level = 0;
			mpf_private.segment_best_frame = -1;
		}
    }
    return gst_pad_event_default(pad, event);
}
/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
	mpf_rdf_add_input("metadata");
	mpf_rdf_add_output("segments");


	mpf_add_param_int("threshold", "Interest Threshold", "Decision threshold to pass media based on level of interest in metadata input",
			0, 100, 50);

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

    mpf_private.threshold = mpf_param_get_int("threshold");

    if (mpf_param_get_int("mpf-debug"))
        mpf_component_get_curcomponent()->flags = MPF_DEBUG;

    GstPad *metadata = gst_element_get_pad(GST_ELEMENT(
            mpf_component_get_curcomponent()), "metadata");
    gst_pad_set_event_function(metadata, pad_event_handler);

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	GrdfGraph* graph;
	GrdfStmt* statement;
	GrdfNode* predicate;

	graph = mpf_rdf_pull("metadata");

	predicate = grdf_node_uri_new(graph, "urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel");
	statement = grdf_graph_find_first_pred_node(graph, predicate);
	int loi = grdf_node_literal_get_int(statement->obj);

	MPF_PRIVATE_DEBUG("loi=%d\n", loi);

	if (loi >= mpf_private.threshold) {

		if (mpf_private.segment_best_level < loi) {
			mpf_private.segment_best_level = loi;
			mpf_private.segment_best_frame = mpf_private.input_frame;
		}
		if (!mpf_private.in_segment) {
			mpf_private.in_segment = TRUE;
			mpf_private.segment_start = mpf_private.input_frame;
		}

	} else {

		if (mpf_private.in_segment) {
			mpf_private.segment_end = mpf_private.input_frame-1;
			GrdfGraph *segment = mpf_rdf_new();
			GrdfNode *node = grdf_node_anon_new(segment, "segment");
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/start_frame", mpf_private.segment_start);
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/end_frame", mpf_private.segment_end);
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/best_frame", mpf_private.segment_best_frame);
			grdf_stmt_new_nui(segment, node, "urn:rdf:appscio.com/ver_1.0/segmenter/best_interest", mpf_private.segment_best_level);

			mpf_rdf_push("segments", segment);
			MPF_PRIVATE_INFO("segments: start_frame=%d, end_frame=%d, frames=%d, best_frame=%d, best_interest=%d\n",
				mpf_private.segment_start, mpf_private.segment_end, mpf_private.segment_end-mpf_private.segment_start+1, mpf_private.segment_best_frame, mpf_private.segment_best_level);
			mpf_private.in_segment = FALSE;
			mpf_private.segment_best_level = 0;
			mpf_private.segment_best_frame = -1;
		}
	}
	mpf_private.input_frame++;
    return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

