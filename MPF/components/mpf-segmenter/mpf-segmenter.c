/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-rdf.h>
#include <mpf/mpf-logging.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-segmenter"
#define COMPONENT_DESC "Segments a media stream based on metadata input"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	int threshold;
	long frame;
	MpfLogger logger;
	int loglevel;
	// TODO: do frame restamping in a new component.
	float fps; // TODO: use caps for rate.
	gboolean new_segment;
	GstFormat format;
    gdouble rate;
    gint eos_count;
    GstPad *output_pad;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

// Event handler for input pad.
static gboolean pad_event_handler(GstPad *pad, GstEvent *event) {

    GstElement *element = gst_pad_get_parent_element(pad);
    MpfComponent *component = MPF_COMPONENT(GST_OBJECT_PARENT(pad));

    MPF_PRIVATE_INFO("pad_event_handler: pad=%s event=%s\n", gst_pad_get_name(pad), gst_event_type_get_name(
            event->type));

    if (event->type == GST_EVENT_EOS) {
    	LOCK();
    	mpf_private.eos_count++;
    	MPF_PRIVATE_DEBUG("eos_count=%d\n", mpf_private.eos_count);
    	if (mpf_private.eos_count == 2) {
    		MPF_PRIVATE_DEBUG("EOS -> output\n");
    		gst_pad_push_event(mpf_private.output_pad, gst_event_new_eos());
    	}
    	UNLOCK();
    	return TRUE;
    }

    if (event->type == GST_EVENT_NEWSEGMENT) {
        gboolean update;
        gint64 start, stop, position;
        gst_event_parse_new_segment (event, &update, &mpf_private.rate, &mpf_private.format, &start, &stop,
            &position);
        MPF_PRIVATE_ALWAYS("new segment: rate=%g, format=%d\n", mpf_private.rate, mpf_private.format);
    }
    return gst_pad_event_default(pad, event);
}


/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
	mpf_voidstar_add_input("input");
	mpf_voidstar_add_output("output");
	mpf_rdf_add_input("metadata");

	mpf_add_param_int("threshold", "Interest Threshold", "Decision threshold to pass media based on level of interest in metadata input",
			0, 100, 50);
	mpf_add_param_int("mpf-debug", "Debug MPF", "0=off, 1=on", 0, 1, 0);
	mpf_add_param_float("fps", "Frames Per Second", "Timestamp buffers as FPS", 0, FLT_MAX, 0);

	mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);

}

/* Component instance initialization and parameter handling. */
component_setup() {
	mpf_private.threshold = mpf_param_get_int("threshold");
	mpf_private.fps = mpf_param_get_float("fps");
	if (mpf_param_get_int("mpf-debug") > 0)
		mpf_component_get_curcomponent()->flags = MPF_DEBUG;

	mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

    GstPad *input = gst_element_get_pad(GST_ELEMENT(
            mpf_component_get_curcomponent()), "input");
    gst_pad_set_event_function(input, pad_event_handler);

    GstPad *metadata = gst_element_get_pad(GST_ELEMENT(
            mpf_component_get_curcomponent()), "metadata");
    gst_pad_set_event_function(metadata, pad_event_handler);

    mpf_private.output_pad = gst_element_get_pad(GST_ELEMENT(
            mpf_component_get_curcomponent()), "output");

	mpf_private.new_segment = TRUE;
	mpf_private.rate = 1.0;
	mpf_private.format = GST_FORMAT_DEFAULT;

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	void* buf;
	void* metadata;
	GrdfGraph* graph;
	GList* statements;
	GrdfStmt* statement;
	GrdfNode* predicate;
	GrdfNode* object;
	gint loi;

	MPF_PRIVATE_DEBUG("<- metadata\n");

	graph = mpf_rdf_pull("metadata");

	if (!graph) {
		MPF_PRIVATE_ERROR("mpf-segmenter null metadata\n");
		return;
	}

	MPF_PRIVATE_DEBUG("<- input\n");

	buf = mpf_voidstar_pull("input");

	if (!buf) {
		MPF_PRIVATE_ERROR("mpf-segmenter null input");
		return;
	}

	MPF_PRIVATE_DEBUG("mpf-segmenter: processing buffer %p\n", buf);

	// Find the level of interest.
	// TODO: statements = grdf_find_pred_node(graph, pred);
	// foreach statement: statements { combine levels into loi }

	if (MPF_PRIVATE_ENABLED(MPF_LEVEL_DEBUG)) {
		grdf_graph_dump(stdout, graph);
	}

	predicate = grdf_node_uri_new(graph, "urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel");
	statement = grdf_graph_find_first_pred_node(graph, predicate);
	loi = grdf_node_literal_get_int(statement->obj);

	MPF_PRIVATE_DEBUG("loi=%d\n", loi);

	// Push the results out to the next component in the pipeline, if threshold is satisfied.
	// if (toi.interest >= mpf_private.threshold)...
	if (loi >= mpf_private.threshold) {
		MPF_PRIVATE_INFO("-> output frame=%d\n", mpf_private.frame);
		long time = component->cur_timestamp;
		// Stomp the timestamp.
		if (mpf_private.fps > 0) {
			component->cur_timestamp = mpf_private.frame*1E9/mpf_private.fps;
			MPF_PRIVATE_INFO("time-stamping component to %lld because fps=%f\n", component->cur_timestamp, mpf_private.fps);
		}
		mpf_private.frame++;

		if (mpf_private.new_segment) {
			// new segment
			MPF_PRIVATE_INFO("sending newsegment event for time: %lld\n", component->cur_timestamp);
			GstPad *outpad = gst_element_get_pad(GST_ELEMENT(component), "output");

			GstEvent *newseg = gst_event_new_new_segment(FALSE, mpf_private.rate, mpf_private.format, component->cur_timestamp, GST_CLOCK_TIME_NONE,
				0);
			MpfComponent *component = mpf_component_get_curcomponent();
			gboolean result = gst_pad_push_event(outpad, newseg);
			if (!result) {
				MPF_PRIVATE_ERROR("gst_pad_push_event(output, newseg) failed\n");
			}
			mpf_private.new_segment = FALSE;
		}
		mpf_voidstar_push("output", buf);
	} else {

		// Chucker.
		MPF_PRIVATE_DEBUG("-> chuck\n");

		mpf_voidstar_unref(buf);
		mpf_private.new_segment = TRUE;
	}
	// We are done with the metadata, unref it.
	grdf_node_unref(predicate);
	fflush(stdout);
	return MPF_GOOD;
}

