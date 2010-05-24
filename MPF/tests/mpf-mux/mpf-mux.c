/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-mux"
#define COMPONENT_DESC "Simple type-less multiplexer."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	// int field1;
	int buffer_count;
	gboolean end_1, end_2;
};

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

// Handle EOS on inputs, send to output only when both inputs are finished.  Otherwise
// race condition will shut pipeline down early.
static gboolean eos_handler(GstPad *pad, GstEvent *event)
{
	GstElement *element = gst_pad_get_parent_element(pad);
	MpfComponent *component = MPF_COMPONENT (GST_OBJECT_PARENT (pad));
	gchar *padname = gst_pad_get_name(pad);
	if (event->type == GST_EVENT_EOS) {
		if (!strcmp("input0", padname)) {
			printf("EOS <- input0\n");
			mpf_private.end_1 = TRUE;
		} else if (!strcmp("input1", padname)){
			printf("EOS <- input1\n");
			mpf_private.end_2 = TRUE;
		}
		if (mpf_private.end_1 && mpf_private.end_2) {
			printf("EOS -> output\n");
			GstPad *output = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "output");
			gst_pad_push_event(output, gst_event_new_eos());
		}
		return TRUE;
	}
	return gst_pad_event_default(pad, event);
}


/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
	mpf_voidstar_add_input("input0");
	mpf_voidstar_add_input("input1");
	mpf_voidstar_add_output("output");
	mpf_add_param_int("debug", "Debug MPF", "0=off, 1=on", 0, 1, 0);

}

/* Component instance initialization and parameter handling. */
component_setup() {
	if (mpf_param_get_int("debug") > 0)
		mpf_component_get_curcomponent()->flags = MPF_DEBUG;
	GstPad *input1 = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input0");
	gst_pad_set_event_function(input1, eos_handler);
	GstPad *input2 = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input1");
	gst_pad_set_event_function(input2, eos_handler);
}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	/* Print a message to show we are doing something */
	mpf_private.buffer_count++;
	fprintf(stdout, "%s buffer %d\n", COMPONENT_NAME, mpf_private.buffer_count);

	/* If the input image is not being sent down the pipeline then release it. */
	// mpf_iplimage_unref(src);

	/* Merge the input buffers onto the output pad. */
	mpf_voidstar_push_pad("output", mpf_voidstar_pull("input0"), NULL);
	mpf_voidstar_push_pad("output", mpf_voidstar_pull("input1"), NULL);


	return MPF_GOOD;
}

