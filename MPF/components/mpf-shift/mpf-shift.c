/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>
// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-shift"
#define COMPONENT_DESC "Applies a time-shift to input buffers."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
    // int field1;
    int buffer_count;
    int loglevel;
    MpfLogger logger;
    gfloat shift;
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


    mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
    mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);

    mpf_add_param_float("shift", "shift", "shift (seconds", 0, FLT_MAX, 0);

}

/* Component instance initialization and parameter handling. */
component_setup() {
    // Initialize logging.
    mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

    if (mpf_param_get_int("mpf-debug"))
        mpf_component_get_curcomponent()->flags = MPF_DEBUG;

    mpf_private.shift = mpf_param_get_float("shift");

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	mpf_private.buffer_count++;
    // fprintf(stdout, "%s buffer %d\n", COMPONENT_NAME, mpf_private.buffer_count);

	gboolean push = FALSE;
	GstClockTime time = component->cur_timestamp;
	GstClockTime shift = mpf_private.shift*1e9;
    // Modify timestsamp.
    if (component->cur_timestamp >= shift) {
    	push = TRUE;
    	component->cur_timestamp -= shift;
    }

    /* If the input image is not being sent down the pipeline then release it. */
    // mpf_iplimage_unref(src);

    /* Push the results out to the next component in the pipeline. */
	void* input = mpf_voidstar_pull("input");
	if (push) {
		MPF_PRIVATE_INFO("push time=%lld, timestamp=%lld\n", (long long int)time, (long long int)component->cur_timestamp);
		mpf_voidstar_push("output", input);
	} else {
		MPF_PRIVATE_DEBUG("shift: chucking input\n");
	}

    return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

