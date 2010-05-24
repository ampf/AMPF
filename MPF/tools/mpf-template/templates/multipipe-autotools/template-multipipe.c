/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
// Include the headers for any data-types needed.
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>

#include <gst/app/gstappsrc.h>
#include <gst/app/gstappsink.h>

#include <string.h>

// These defines provide identification information for the component.
#define COMPONENT_NAME "template-multipipe"
#define COMPONENT_DESC "MPF Multi-Pipe Example"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

// Define as 1 to do a test for a sub-pipeline that has a videobalance.  The brightness will be changed
// in real-time.  Remove blocks enabled by this macro when creating your own components.
#define TEST_VIDEOBALANCE 1

// This structure stores data for each component instance.
// Access field1 as mpf_private.field1
struct component_private {
    int buffer_count;
    int loglevel;
    MpfLogger logger;
    GstPipeline *pipeline;
    GstAppSrc *appsrc;
    GstAppSink *appsink;
    GstFormat format;
    const gchar *launch;
    gboolean new_segment;
    gint segment, segment_count;
    gboolean one_pipeline;
    glong sleep;
#if TEST_VIDEOBALANCE
    GstElement *videobalance;
#endif
};

// Include the component template header
#include <mpf/mpf-componenttemplate.h>

// Bus handler for the sub-pipeline
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data) {

    MpfComponent *component = (MpfComponent*) data;

    switch (GST_MESSAGE_TYPE(msg)) {

    case GST_MESSAGE_EOS:
        MPF_PRIVATE_INFO("bus_call: %s EOS\n", GST_ELEMENT_NAME(bus));
        break;
    case GST_MESSAGE_STATE_CHANGED: {
        GstState old, new, pending;
        gst_message_parse_state_changed(msg, &old, &new, &pending);
        MPF_PRIVATE_DEBUG("bus_call: %s state-changed: %s old=%s, new=%s, pending=%s\n", GST_ELEMENT_NAME(bus), GST_MESSAGE_SRC_NAME(msg), gst_element_state_get_name(old),
            gst_element_state_get_name(new), gst_element_state_get_name(pending));
        break;
    }
    case GST_MESSAGE_ERROR: {
        gchar *debug;
        GError *error;

        gst_message_parse_error(msg, &error, &debug);
        g_free(debug);

        MPF_PRIVATE_ERROR("bus_call: Error: %s\n", error->message);
        g_error_free(error);

        break;
    }
    default:
        MPF_PRIVATE_DEBUG("bus_call: %s %s\n",GST_ELEMENT_NAME(bus),  GST_MESSAGE_TYPE_NAME(msg));
        break;
    }

    return TRUE;
}

static gint compare_by_name(gpointer *data, gpointer *user) {
    GstElement *element = (GstElement*)data;
    gchar* name = (gchar*)user;
    return !strstr(GST_ELEMENT_NAME(element), name);
}

static GstElement* find_by_name(MpfComponent* component, gchar* name) {
    GstIterator *iter;
    iter = gst_bin_iterate_elements(GST_BIN(mpf_private.pipeline));
    return gst_iterator_find_custom(iter, (GCompareFunc)compare_by_name, name);
}

// Create and start a sub-pipeline.
static void start_sub_pipeline(MpfComponent *component) {

    g_assert(mpf_private.pipeline == NULL);

    // Substitute segment into launch line. TODO: do real substitution, e.g. ${seg}.
    gchar* launch = g_strdup_printf(mpf_private.launch, mpf_private.segment);

    GError *error = NULL;

    mpf_private.pipeline = (GstPipeline*)gst_parse_launch(launch, &error);

    if (error != NULL) {
        MPF_PRIVATE_ERROR("Error: %s\n", error->message);
    }

#if TEST_VIDEOBALANCE
    mpf_private.videobalance = find_by_name(component, "videobalance");
#endif

    g_assert(mpf_private.pipeline != NULL);

    // Monitor bus messages in the sub-pipeline.
    GstBus *bus = gst_pipeline_get_bus(GST_PIPELINE(mpf_private.pipeline));
    MPF_PRIVATE_DEBUG("adding bus watch %s\n", GST_OBJECT_NAME(bus));
    gst_bus_add_watch(bus, bus_call, component);
    gst_object_unref(bus);

    // Reasonable restriction: at most one appsrc, one appsink, named as defaults.
    mpf_private.appsrc = (GstAppSrc*)find_by_name(component, "appsrc");
    mpf_private.appsink = (GstAppSink*)find_by_name(component, "appsink");

    MPF_PRIVATE_INFO("appsrc=%p, appsink=%p\n", mpf_private.appsrc, mpf_private.appsink);

    GstStateChangeReturn state = gst_element_set_state(GST_ELEMENT(mpf_private.pipeline),
            GST_STATE_PLAYING);
    MPF_PRIVATE_DEBUG("sub-pipeline state=%s\n", gst_element_state_change_return_get_name(state));

}

static void stop_sub_pipeline(MpfComponent *component) {
    if (mpf_private.pipeline) {
        gst_element_set_state(GST_ELEMENT(mpf_private.pipeline), GST_STATE_NULL);
        gst_object_unref (GST_OBJECT (mpf_private.pipeline));
        if (mpf_private.appsrc) {
            MPF_PRIVATE_INFO("EOS -> %s\n", GST_ELEMENT_NAME(mpf_private.appsrc));
            gst_app_src_end_of_stream(mpf_private.appsrc);
        }
        gst_element_send_event(GST_ELEMENT(mpf_private.pipeline), gst_event_new_eos());
        // TODO: wait for sub-pipeline to flush.  This is a hack, needs to be fixed.
        MPF_PRIVATE_WARN("waiting %d us for pipeline %s...\n", mpf_private.sleep, GST_ELEMENT_NAME(mpf_private.pipeline));
        usleep(mpf_private.sleep);
        mpf_private.pipeline = NULL;
    }
}

// Event handler for input pad in parent component, propagates events to sub-pipeline.
static gboolean pad_event_handler(GstPad *pad, GstEvent *event) {

    GstElement *element = gst_pad_get_parent_element(pad);
    MpfComponent *component = MPF_COMPONENT(GST_OBJECT_PARENT(pad));
    // If EOS, poke a message out of the events pad.

    MPF_PRIVATE_INFO("pad_event_handler: %s\n", gst_event_type_get_name(
            event->type));

    if (event->type == GST_EVENT_EOS) {
        stop_sub_pipeline(component);
        GstPad *output = gst_element_get_pad(element, "output");
        gst_pad_push_event(output, event);
        return TRUE;
    }
    if (event->type == GST_EVENT_NEWSEGMENT) {
        GstPad *output = gst_element_get_pad(element, "output");
        gst_pad_push_event(output, event);
        if (mpf_private.segment_count) {
            if (!mpf_private.one_pipeline)
                stop_sub_pipeline(component);
            mpf_private.segment++;
            if (!mpf_private.pipeline || !mpf_private.one_pipeline)
                start_sub_pipeline(component);
        }
        gboolean update;
        gdouble rate;
        gint64 start, stop, position;
        gst_event_parse_new_segment (event, &update, &rate, &mpf_private.format, &start, &stop,
            &position);
        mpf_private.new_segment = TRUE;
        MPF_PRIVATE_INFO("NEWSEGMENT %d\n", mpf_private.segment);
        GstEvent *ev = gst_event_new_new_segment(update, rate, mpf_private.format, start, stop, position);
        gst_element_send_event(GST_ELEMENT(mpf_private.pipeline), ev);
        return TRUE;
    }
    return gst_pad_event_default(pad, event);
}

// Register inputs and outputs of the component.
// Register any parameters.
// Load common datasets.
component_class_init() {
    mpf_voidstar_add_input("input");
    mpf_voidstar_add_output("output");

    mpf_add_param_int("loglevel", "Log Level",
        "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG,
        MPF_LEVEL_ERROR, MPF_LEVEL_WARN);
    mpf_add_param_int("mpf-debug", "Mpf Debug", "Debug MPF component", 0, 1, 0);

    mpf_add_param_string("pipeline", "Pipeline", "Sub-pipeline to run", "appsrc ! appsink");

    mpf_add_param_enum("one-pipeline", "One Pipeline", "Process all segments using one sub-pipeline", "false",
        "false", "Use one pipeline for all segments",
        "true", "Start a new pipeline for each segment", NULL);

    mpf_add_param_int("sleep", "Sleep", "How long to wait for sub-pipeline shutdown (microseconds)", 0, INT_MAX, 1000000);
}

// Component instance initialization and parameter handling.
component_setup() {

    GstElement *element = GST_ELEMENT(component);

    // component->flags = MPF_DEBUG;

    // Initialize logging.
    mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel,
            gst_element_get_name(element));

    if (mpf_param_get_int("mpf-debug"))
        mpf_component_get_curcomponent()->flags = MPF_DEBUG;

    mpf_private.launch = mpf_param_get_string("pipeline");

    GstPad *input = gst_element_get_pad(GST_ELEMENT(
            mpf_component_get_curcomponent()), "input");
    gst_pad_set_event_function(input, pad_event_handler);

    mpf_private.one_pipeline = mpf_param_get_enum("one-pipeline");
    mpf_private.sleep = mpf_param_get_int("sleep");

    start_sub_pipeline(component);

}

// The main process function is called once per "frame".
// All the buffers at the same timestamp from each of the inputs are available.
component_process() {
    // Print a message to show we are doing something
    mpf_private.buffer_count++;
    MPF_PRIVATE_DEBUG("%s buffer %d\n", COMPONENT_NAME,
            mpf_private.buffer_count);

    mpf_private.segment_count++;

    void* input = mpf_voidstar_pull("input");
    GstBuffer *buf = mpf_voidstar_get_buffer(NULL, input);

    if (mpf_private.new_segment && mpf_private.appsrc) {
        MPF_PRIVATE_INFO("sending new caps into appsrc\n");
        gst_app_src_set_caps(mpf_private.appsrc, mpf_buffer_get_caps(input));
        mpf_private.new_segment = FALSE;
    }

    if (mpf_private.appsrc) {
        // Send the buffer in for processing.
        MPF_PRIVATE_INFO("input -> %s\n", GST_ELEMENT_NAME(mpf_private.appsrc));
        gst_app_src_push_buffer(mpf_private.appsrc, buf);
    }

#if TEST_VIDEOBALANCE
    // Ramp a videobalance brightness up and down.
    if (mpf_private.videobalance) {
        float brightness = (float) (mpf_private.buffer_count % 100) / 50;
        brightness -= 1;
        g_object_set(G_OBJECT(mpf_private.videobalance), "brightness", brightness,
                NULL);
    }
#endif

    if (mpf_private.appsink) {
        MPF_PRIVATE_INFO("%s -> output\n", GST_ELEMENT_NAME(mpf_private.appsink));
        // Get the return buffer.
        buf = gst_app_sink_pull_buffer(mpf_private.appsink);
        g_assert(buf != NULL);
        // Send it out.
        mpf_component_push_buffer(NULL, "output", buf);
    }

    return MPF_GOOD;
}

// Component instance finalization.
component_teardown() {
}

