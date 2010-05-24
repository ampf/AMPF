/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>
// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-identity"
#define COMPONENT_DESC "Template component that passes data through unmodified"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

#define NANOS 1000000000L

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	// int field1;
	int buffer_count;
	int loglevel;
	MpfLogger logger;
	float sleep;
	float restamp;
	int num_buffers;
	float timeout;
	GstClockTime firsttime;
	double cfps;
	double afps;
	double start_time;
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

	mpf_add_param_float("sleep", "Sleep", "Sleep (seconds)", 0, FLT_MAX, 0);
	mpf_add_param_enum("dump", "Dump mode", "Show buffer/data information.",
			"none", "none", "No dump", "buffers", "Show buffer count and size",
			"dot", "Show dot", NULL);
	// TODO: should be fraction.
	mpf_add_param_float("restamp", "Restamp buffers",
			"Restamp buffers with given FPS", 0, FLT_MAX, 0);
	mpf_add_param_int("num-buffers", "NumBuffers",
			"Number of buffers after which an end of stream is sent", 0,
			INT_MAX, 0);
	mpf_add_param_float("timeout", "Timeout",
			"Send EOS after buffer time exceeds timeout seconds", 0, FLT_MAX, 0);
	mpf_add_param_int("loglevel", "Log Level",
			"Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG,
			MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
}

static double curtime(void) {
	static struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec / 1000000.0;
}

/* Component instance initialization and parameter handling. */
component_setup() {
	mpf_private.sleep = mpf_param_get_float("sleep");
	mpf_private.restamp = mpf_param_get_float("restamp");
	mpf_private.num_buffers = mpf_param_get_int("num-buffers");
	mpf_private.timeout = mpf_param_get_float("timeout");
	mpf_private.start_time = curtime();
	// Initialize logging.
	mpf_private.loglevel = mpf_param_get_int("loglevel");
	mpf_logger_init(&mpf_private.logger, mpf_private.loglevel,
			gst_element_get_name(GST_ELEMENT(component)));
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
	void* input = mpf_voidstar_pull("input");
	mpf_private.buffer_count++;

#if 0
	GstClockTime basetime = gst_element_get_base_time(GST_ELEMENT(&component->element));
	GstObject *parent = GST_OBJECT_PARENT(GST_ELEMENT(&component->element));
	// printf("parent=%p\n", parent);
	while (G_OBJECT_TYPE(parent) != GST_TYPE_PIPELINE) {
		parent = GST_OBJECT_PARENT(parent);
		// printf("parent=%p\n", parent);
	}
	GstPipeline *pipeline = GST_PIPELINE_CAST(parent);
	// printf("pipeline=%p\n", pipeline);
	GstClockTime gsttime = gst_pipeline_get_clock(pipeline);
	GstClockTime elapsed = gsttime - basetime;
	printf("gsttime=%lld, basetime=%lld, elapsed=%lld\n", gsttime, basetime, elapsed);
#endif

	GstClockTime time = mpf_buffer_get_timestamp(input);
	if (mpf_private.firsttime == 0) {
		mpf_private.firsttime = time;
	}
	GstClockTime delta = time - mpf_private.firsttime;
	if (delta > 0) {
		double d = (double) delta;
		mpf_private.cfps = mpf_private.buffer_count / (((double) delta) / 1e9);
	}
	mpf_private.afps = (double) mpf_private.buffer_count / (curtime()
			- mpf_private.start_time);

	if (mpf_private.restamp) {

		// Restamp with provided FPS.

		MPF_PRIVATE_DEBUG("restamping to %g FPS\n", mpf_private.restamp);
		// Stomp the timestamp.
		component->cur_timestamp = mpf_private.buffer_count/mpf_private.restamp*NANOS;
	}

	if (mpf_param_get_enum("dump") == 1) {
		MPF_PRIVATE_ALWAYS(
				"buffer %d (size=%d, time=%2.4g, cfps=%2.4g, afps=%2.4g)\n",
				mpf_private.buffer_count, mpf_voidstar_get_length(input), (double)component->cur_timestamp/NANOS,
				mpf_private.cfps, mpf_private.afps);
	} else if (mpf_param_get_enum("dump") == 2) {
		printf(".");
		fflush(stdout);
	}
	if (mpf_private.sleep) {
		glong sleep = mpf_private.sleep * 1000000;
		usleep(sleep);
		printf("s");
		fflush(stdout);
		// Compute framerate that corresponds to sleep time.
		float sleeprate = 1.0 / mpf_private.sleep;
		if (sleeprate > 1000)
			sleeprate = 1000;

		if (mpf_buffer_get_caps(input)) {
			// Set forward-caps framerate to sleeprate, millisecond resolution.
			mpf_buffer_set_fraction(input, "framerate", 1000, 1000 * sleeprate);
		}
	}
	/* If the input image is not being sent down the pipeline then release it. */
	// mpf_iplimage_unref(src);

	double elapsed = curtime() - mpf_private.start_time;

	if (mpf_private.num_buffers > 0 && mpf_private.buffer_count
			> mpf_private.num_buffers || mpf_private.timeout > 0 && elapsed
			>= mpf_private.timeout) {
		MPF_PRIVATE_WARN(
				"Sending EOS because of timeout or max buffers exceeded\n");
		// Signal end of segment.
		GstPad *output = gst_element_get_pad(GST_ELEMENT(&component->element),
				"output");
		gst_pad_push_event(output, gst_event_new_eos());
	} else {
		/* Push the results out to the next component in the pipeline. */
		mpf_voidstar_push("output", input);
	}

	return MPF_GOOD;
}

