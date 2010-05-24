/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-rdf.h>
#include <mpf/mpf-logging.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-n3-toi"
#define COMPONENT_DESC "Template component that passes data through unmodified"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	gchar *n3;
	gint frame;
	gint n_segments;
	gint segment_index;
	gint segment;
	gint *segment_start;
	gint *segment_end;
	gint *segment_best;
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
	mpf_rdf_add_output("interest");
	mpf_rdf_add_output("best");
	mpf_voidstar_add_input("input");

	mpf_add_param_string("n3", "N3 filename", "N3 file containing time cues", "");
	mpf_add_param_int("clip", "Clip", "Clip length (seconds)", 0, INT_MAX, 10);
	mpf_add_param_int("segment", "Segment", "Pass given segment only", 0, INT_MAX, 0);

	mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);

}

/* Component instance initialization and parameter handling. */
component_setup() {
	mpf_private.n3 = mpf_param_get_string("n3");  // metadata.
	mpf_private.segment = mpf_param_get_int("segment");

	mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

    gchar *buf;
	gsize len;
	GError *err = NULL;
	gboolean ok = g_file_get_contents(mpf_private.n3, &buf, &len, &err);
	// TODO: handle failure.
	// Scan buffer line-by-line.
	gchar **lines = g_strsplit(buf, "\n", 0);
	gint i = 0;

	gchar *subj = 0;
	for (; lines && lines[i] && *lines[i]; i++) {
		MPF_PRIVATE_DEBUG("%d: %s\n", i, lines[i]);
		// Count the records by looking for subjects.
		gchar **triples = g_strsplit(lines[i], " ", 0);
		if (!subj || strcmp(subj, triples[0])) {
			mpf_private.n_segments++;
		}
		subj = triples[0];
	}
	MPF_PRIVATE_INFO("found %d N3 graphs\n", mpf_private.n_segments);
	// Allocate start & end frame storage.
	mpf_private.segment_start = g_malloc(mpf_private.n_segments*sizeof(long));
	mpf_private.segment_end = g_malloc(mpf_private.n_segments*sizeof(long));
	mpf_private.segment_best = g_malloc(mpf_private.n_segments*sizeof(long));
	// Build up start/end segments base on N3
	i = 0;
	int frame = 0;
	subj = 0;
	for (; lines && lines[i] && *lines[i]; i++) {
		// Split lines into <subj, pred, obj> triples>
		gchar **triples = g_strsplit(lines[i], " ", 0);
		MPF_PRIVATE_DEBUG("subj: %s, pred: %s, obj: %s\n", triples[0], triples[1], triples[2]);
		// Compare predicates to extract frames.  TODO: the comparison should be done on fully
		// namespaced predicates.  In this case this would be urn:rdf:appscio.com/ver_2.4/roi/start_frame
		// but a translation would be required from the face-finder namespace which is
		// urn:rdf:appscio.com/partners/pittpatt/ver_2.4/roi/start_frame.  This gets into predicate mapping
		// which is too complex for this prototype.
		// Count the records by looking for subjects.
		if (g_strrstr(triples[1], "start_frame")) {
			sscanf(triples[2], "\"%d\".", &mpf_private.segment_start[frame]);
		} else if (g_strrstr(triples[1], "end_frame")) {
			sscanf(triples[2], "\"%d\".", &mpf_private.segment_end[frame]);
		} else if (g_strrstr(triples[1], "best_frame")) {
			sscanf(triples[2], "\"%d\".", &mpf_private.segment_best[frame]);
		}
		if (!subj) {
			subj = triples[0];
		}
		if (strcmp(subj, triples[0])) {
			frame++;
		}
		subj = triples[0];
	}
	if (MPF_PRIVATE_ENABLED(MPF_LEVEL_INFO)) {
		i = 0;
		for (; i<=frame; i++) {
			MPF_PRIVATE_INFO("%d: start_frame=%d, end_frame=%d\n", i, mpf_private.segment_start[i], mpf_private.segment_end[i]);
		}
	}

	g_free(buf);

}

void breakpoint() {
	return;
}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	breakpoint();

	/* Push the results out to the next component in the pipeline. */
	void* input = mpf_voidstar_pull("input");
	int interest = 0;
	int index = mpf_private.segment_index;
	long start = mpf_private.segment_start[index];
	long end = mpf_private.segment_end[index];
	long best = mpf_private.segment_best[index];
	if (!best)
		best = (start+end)/2;
	glong time = mpf_buffer_get_timestamp(input);

	if (mpf_private.segment > mpf_private.n_segments) {
  	    GstPad *pad = gst_element_get_pad(GST_ELEMENT(&component->element), "interest");
	    gst_pad_push_event(pad, gst_event_new_eos());
  	    pad = gst_element_get_pad(GST_ELEMENT(&component->element), "best");
	    gst_pad_push_event(pad, gst_event_new_eos());
	    return MPF_GOOD;
	}

	// TODO: what if segment_start/segment_end regions overlap?
	if (start <= mpf_private.frame && mpf_private.frame <= end) {
		if (mpf_private.segment == 0 || mpf_private.segment == mpf_private.segment_index+1) {
		    interest = 100;
		}
	} else if (mpf_private.segment_index < mpf_private.n_segments && mpf_private.frame > end) {
		mpf_private.segment_index++;
		MPF_PRIVATE_DEBUG("segment_index=%d\n", mpf_private.segment_index);
	}

	MPF_PRIVATE_DEBUG("time=%lld frame=%d, segment_index=%d, segment_start=%d, segment_end=%d, interest=%d\n", time, mpf_private.frame, mpf_private.segment_index, start, end, interest);

	GrdfGraph *graph = mpf_rdf_new();
	GrdfNode *node = grdf_node_anon_new(graph, "interest");
	grdf_stmt_new_nuu(graph, node, "dcterms:type","urn:rdf:appscio.com/ver_1.0/toi");
	grdf_stmt_new_nui(graph, node, "urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel", interest);

	mpf_private.frame++;

	mpf_rdf_push("interest", graph);

	{
		interest = 0;
		GrdfGraph *graph = mpf_rdf_new();
		GrdfNode *node = grdf_node_anon_new(graph, "interest");
		grdf_stmt_new_nuu(graph, node, "dcterms:type","urn:rdf:appscio.com/ver_1.0/toi");
		if (mpf_private.frame == best) {
			interest = 100;
			MPF_PRIVATE_INFO("time=%lld frame=%d, segment_index=%d, segment_start=%d, segment_end=%d, best=%d\n", time, mpf_private.frame, mpf_private.segment_index, start, end, best);
		}
		grdf_stmt_new_nui(graph, node, "urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel", interest);
		mpf_rdf_push("best", graph);
	}

	mpf_voidstar_unref(input);

	return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

