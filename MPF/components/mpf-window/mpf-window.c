/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>

// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-window"
#define COMPONENT_DESC "Samples a stream, buffer by buffer, start, count"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
	// int field1;
	int buffer_count;
	int start;
	int count;
	int repeat;
	int preserve;
	int pushed;
	MpfLogger logger;
	int loglevel;
    gdouble rate;
	GstFormat format;
	gboolean new_segment;
};

static gboolean
input_events(GstPad *pad, GstEvent *event);

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {

	mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
	mpf_voidstar_add_input("input");
	mpf_voidstar_add_output("output");
	mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);
	mpf_add_param_int("start", "Start", "Start buffer", 0, INT_MAX, 0);
	mpf_add_param_int("count", "Count", "Number of buffers", -1, INT_MAX, -1);
	mpf_add_param_int("repeat", "Repeat", "Repeat buffers after...", 0, INT_MAX, 0);
	mpf_add_param_enum("preserve", "Preserve timestamps", "Preserve incoming timestamps or reset them to NONE?", "true",
			"false", "Set timestamps to -1",
			"true", "Preserve timestamps.", NULL);

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
	if (mpf_param_get_int("mpf-debug"))
		mpf_component_get_curcomponent()->flags = MPF_DEBUG;
	mpf_private.start = mpf_param_get_int("start");
	mpf_private.count = mpf_param_get_int("count");
	mpf_private.repeat = mpf_param_get_int("repeat");
	mpf_private.preserve = mpf_param_get_enum("preserve");

	mpf_private.loglevel = mpf_param_get_int("loglevel");
	mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

	GstPad *pad = gst_element_get_pad(GST_ELEMENT(component), "input");
	gst_pad_set_event_function(pad, input_events);

	mpf_private.new_segment = TRUE;
	mpf_private.rate = 1.0;
	mpf_private.format = GST_FORMAT_DEFAULT;

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {

	// Windowing operations.

	// +++++++++....................++++++++++.......... start=0 count=10 repeat=30
	// .........++++++++++....................++++++++++ start=9 count=10 repeat=30
	// ...................++++++++++.................... start=19 count=10 repeat=30

	// Ovelapped
	// +++++++++++++++...............+++++++++++++++..... start=0 count=15 repeat=30
	// ..........+++++++++++++++...............++++++++++ start=9 count=15 repeat=30
	// ....................+++++++++++++++............... start=19 count=15 repeat=30
	/* Push the results out to the next component in the pipeline. */
	int mod = (mpf_private.buffer_count - mpf_private.start)%(mpf_private.repeat?mpf_private.repeat:INT_MAX);

	MPF_LOG_DEBUG(&mpf_private.logger, "mod=%d\n", mod);
	void *in = mpf_voidstar_pull("input");

	if (mpf_private.buffer_count >= mpf_private.start &&  (mpf_private.count < 0 || (mod >= 0 && mod < mpf_private.count))) {
		MPF_LOG_INFO(&mpf_private.logger, "buffer %d (%d)\n", mpf_private.buffer_count, mpf_private.pushed);
		if (!mpf_private.preserve) {
			// TODO: this is all or nothing on the output pads, per component.  Should be per pad.
			component->cur_timestamp = GST_CLOCK_TIME_NONE;
		}
		mpf_private.pushed++;
		mpf_voidstar_push("output", in);

		if (mpf_private.new_segment) {
			// Send new-segment event to downstream component.
			MPF_PRIVATE_DEBUG("pushing new-segment event to output pad\n");
			GstEvent *event = gst_event_new_new_segment(FALSE, mpf_private.rate, mpf_private.format, component->cur_timestamp, GST_CLOCK_TIME_NONE, 0);
			GstPad *pad = gst_element_get_pad(GST_ELEMENT(&component->element), "output");
			gst_pad_push_event (pad, event);
			mpf_private.new_segment = FALSE;
		}

	} else {
		MPF_LOG_DEBUG(&mpf_private.logger, "discard %d\n", mpf_private.buffer_count);
		mpf_private.new_segment = TRUE;
	}
	mpf_private.buffer_count++;


	return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
}

static gboolean
input_events(GstPad *pad, GstEvent *event)
{
	MpfComponent *component = MPF_COMPONENT (GST_OBJECT_PARENT (pad));
	GstElement *element = gst_pad_get_parent_element(pad);
	gchar *elementname = gst_element_get_name(element);
	gchar *padname = gst_pad_get_name(pad);
	const gchar *eventname = gst_event_type_get_name(event->type);

	MPF_LOG_INFO(&mpf_private.logger, "%s: pad=%s event=%s\n", elementname, padname, eventname);

	if (GST_EVENT_TYPE (event) == GST_EVENT_NEWSEGMENT) {
		gboolean update;
		gdouble rate;
		gint64 start, stop, position;
		gst_event_parse_new_segment (event, &update, &mpf_private.rate, &mpf_private.format, &start, &stop,
				&position);
		MPF_LOG_INFO(&mpf_private.logger, "format=%d, start=%lld, stop=%lld, position=%lld\n", mpf_private.format, start, stop, position);
	}

	return gst_pad_event_default(pad, event);
}


