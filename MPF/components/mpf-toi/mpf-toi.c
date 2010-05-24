/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-rdf.h>
#include <float.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-toi"
#define COMPONENT_DESC "Time of interest component that passes data through unmodified and provides an interest level based on the stream time."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	// int field1;
	int buffer_count;
	float start;
	float interval;
	float duration;
	float end;
	int normalize;
	long firsttime;
	int in;
	int out;
	int verbose;
	int oldtestm; // Last value of testm to track duration=0 cycles.
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */

component_class_init() {
	mpf_voidstar_add_input("input");
	mpf_rdf_add_output("interest");

	/* Add parameters (name, nickname, description, min, max, default) */
	mpf_add_param_float("start", "Start Time", "Start time in seconds.", 0,
			FLT_MAX, 0);
	// interval 0 means it does not repeat
	mpf_add_param_float("interval", "Interval Time",
			"Interval time in seconds.", 0, FLT_MAX, 0);
	// duration 0 means 1 buffer only
	mpf_add_param_float("duration", "Duration Time",
			"Duration time in seconds.", 0, FLT_MAX, 0);
	// end -1 means end of stream
	mpf_add_param_float("end", "End Time", "End time in seconds.", -1, FLT_MAX,
			-1);
	mpf_add_param_int("normalize", "Zero time",
			"Treat first buffer as zero time", 0, 1, 1);
	// 100 max interest level
	mpf_add_param_int("in", "In Value", "In interest level value.", 0, 100, 100);
	// 0 min interest level
	mpf_add_param_int("out", "Out Value", "Out interest level value.", 0, 100,
			0);
	mpf_add_param_int("verbose", "Verbosity", "0=off, 1=info, 2=noisy", 0, 2, 0);
	mpf_add_param_int("debug", "Debug MPF", "0=off, 1=on", 0, 1, 0);
}

/* Component instance initialization and parameter handling. */
component_setup() {
	mpf_private.start = mpf_param_get_float("start");
	mpf_private.interval = mpf_param_get_float("interval");
	mpf_private.duration = mpf_param_get_float("duration");
	mpf_private.end = mpf_param_get_float("end");
	mpf_private.normalize = mpf_param_get_int("normalize");
	mpf_private.in = mpf_param_get_int("in");
	mpf_private.out = mpf_param_get_int("out");
	mpf_private.verbose = mpf_param_get_int("verbose");
	if (mpf_param_get_int("debug") > 0)
		mpf_component_get_curcomponent()->flags = MPF_DEBUG;
	mpf_private.oldtestm = -1; // Signal first cycle.
	mpf_private.firsttime = -1;
}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	void *buf;
	float time, test;
	long millis;
	int interest, testm, intervalm, durationm;

	buf = mpf_voidstar_pull("input");

	millis = mpf_buffer_get_timestamp(buf) / 1000000L;
	if (mpf_private.firsttime == -1) {
		mpf_private.firsttime = millis;
	}
	// Normalize time to zero.
	if (mpf_private.normalize) {
		millis -= mpf_private.firsttime;
	}
	time = ((float) millis) / 1000.;
	if (mpf_private.verbose >= 2) {
		fprintf(stdout,
				"start: %f end: %f interval: %f duration: %f in: %d out: %d\n",
				mpf_private.start, mpf_private.end, mpf_private.interval,
				mpf_private.duration, mpf_private.in, mpf_private.out);
	}

	if (time < mpf_private.start) {
		interest = mpf_private.out;
	} else if ((mpf_private.end != -1) && (time >= mpf_private.end)) {
		interest = mpf_private.out;
	} else {
		test = time - mpf_private.start;
		testm = (int) (test * 1000.);
		intervalm = (int) (mpf_private.interval * 1000.);
		durationm = (int) ((mpf_private.duration) * 1000.);
		if (intervalm > 0) {
			testm %= intervalm;
		}
		if (durationm == 0) {
			if (intervalm == 0 || mpf_private.oldtestm < 0 || testm
					< mpf_private.oldtestm) {
				interest = mpf_private.in;
			} else {
				interest = mpf_private.out;
			}
		} else {
			if (testm < durationm) {
				interest = mpf_private.in;
			} else {
				interest = mpf_private.out;
			}
		}
		mpf_private.oldtestm = testm; // Cannot be less than zero, hence initial value of -1 to act as sentinel.
	}

	gchar *buftime = g_strdup_printf("%lld", mpf_buffer_get_timestamp(buf));

	GrdfGraph *graph = mpf_rdf_new();
	GrdfNode *node = grdf_node_anon_new(graph, "interest");
	grdf_stmt_new_nuu(graph, node, "dcterms:type",
			"urn:rdf:appscio.com/ver_1.0/toi");
	grdf_stmt_new_nui(graph, node,
			"urn:rdf:appscio.com/ver_1.0/toi/currentInterestLevel", interest);
	grdf_stmt_new_nul(graph, node, "urn:rdf:appscio.com/ver_1.0/toi/timestamp",
			buftime);

	if (mpf_private.verbose >= 1)
		fprintf(stdout, "%s buffer: %d time: %f interest: %d\n",
				COMPONENT_NAME, mpf_private.buffer_count, time, interest);

	mpf_private.buffer_count++;
	mpf_rdf_push("interest", graph);

	mpf_voidstar_unref(buf);

	return MPF_GOOD;
}

