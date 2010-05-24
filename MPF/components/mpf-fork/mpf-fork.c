/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Include the headers for any data-types needed. */
#include <mpf/mpf-voidstar.h>
#include <mpf/mpf-logging.h>
#include <fcntl.h>

// #include <mpf/mpf-iplimage.h>
// #include <mpf/mpf-rdf.h>

/* These defines provide identification information for the component. */
#define COMPONENT_NAME "mpf-fork"
#define COMPONENT_DESC "Links pipeline components to an external process using named pipes."
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

#if !defined(htobe64)
// This will NOT work for big-endian machines. It is in place to support SUSE 10.2/glibc 2.4 which does
// not define the htobe64/be64toh macros.
# define __bswap_constant_64(x) \
     ((((x) & 0xff00000000000000ull) >> 56)                   \
      | (((x) & 0x00ff000000000000ull) >> 40)                     \
      | (((x) & 0x0000ff0000000000ull) >> 24)                     \
      | (((x) & 0x000000ff00000000ull) >> 8)                      \
      | (((x) & 0x00000000ff000000ull) << 8)                      \
      | (((x) & 0x0000000000ff0000ull) << 24)                     \
      | (((x) & 0x000000000000ff00ull) << 40)                     \
      | (((x) & 0x00000000000000ffull) << 56))

#define htobe64(x) __bswap_constant_64(x)
#define be64toh(x) __bswap_constant_64(x)
#endif

/* This structure stores data for each component instance. */
/* Access field1 as mpf_private.field1 */
struct component_private {
  // int field1;
  int buffer_count;
  int wrap;
  gchar *sink, *src;
  FILE *readPipe;
  FILE *writePipe;
  gboolean close;
  MpfLogger logger;
  int loglevel;
  gboolean sink_first;
  int size;
};

// How buffers are wrapped. Basically GstBuffer minus the things we can't sensibly send.
// TODO: handle caps.
#pragma pack(4)
typedef struct _Wrapper {
    guint size;
    GstClockTime timestamp;
    GstClockTime duration;
} Wrapper;
#pragma options align=reset

static void pipe_reader(void* data);
static gboolean pad_event_handler(GstPad* pad, GstEvent* event);

/* Include the component template header */
#include <mpf/mpf-componenttemplate.h>

/* Register inputs and outputs of the component.
 * Register any parameters.
 * Load common datasets.
 */
component_class_init() {
  mpf_voidstar_add_input("input");
  mpf_voidstar_add_output("output");

  mpf_add_param_string("command", "Command", "Forked command with pipe substitution", "cat < %s > %s");

  mpf_add_param_enum("mode", "Mode", "How to talk to the pipes.", "raw",
      "raw", "Send buffer without wrapper",
      "wrap", "Wrap buffers with 8 byte length field.", NULL);

  mpf_add_param_boolean("close", "Close", "Close pipes between buffers", FALSE);
  mpf_add_param_boolean("sink-first", "Sink First", "Open the sink pipe first", FALSE);
  mpf_add_param_int("size", "Size", "Size of buffer to read from pipe (bytes)", 1, INT_MAX, 256);

  mpf_add_param_int("loglevel", "Log Level", "Logging level (0-3 = DEBUG .. ERROR)", MPF_LEVEL_DEBUG, MPF_LEVEL_ERROR, MPF_LEVEL_WARN);

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
    // mpf_component_get_curcomponent()->flags = MPF_DEBUG;

    mpf_private.wrap = mpf_param_get_enum("mode");
    mpf_private.close = mpf_param_get_boolean("close");
    mpf_private.sink_first = mpf_param_get_boolean("sink-first");
    mpf_private.size = mpf_param_get_int("size");

    // Initialize logging.
    mpf_private.loglevel = mpf_param_get_int("loglevel");
    mpf_logger_init(&mpf_private.logger, mpf_private.loglevel, gst_element_get_name(GST_ELEMENT(component)));

    // Setup named pipes based on actual component name and process id, which will be
    // unique.
    gchar *name = gst_element_get_name(GST_ELEMENT(component));
    MPF_LOG_DEBUG(&mpf_private.logger,"%s\n", name);

    int mask = umask(0);
    mkdir("/tmp/appscio-mpf", S_IRWXU | S_IRWXG | S_IRWXO);
    umask(mask);

    // TODO: Make fifos based on pad names.
    // TODO: Not totally safe -- should key based on process id etc.
    gchar* sink = tempnam("/tmp/appscio-mpf", "sink-");
    gchar* src = tempnam("/tmp/appscio-mpf", "src-");
    mpf_private.sink = sink = g_strdup_printf("%s-#%d", sink, getpid());
    mpf_private.src = src = g_strdup_printf("%s-#%d", src, getpid());
    MPF_LOG_DEBUG(&mpf_private.logger,"sink=%s\n", sink);
    MPF_LOG_DEBUG(&mpf_private.logger,"src=%s\n", src);
    mkfifo(sink, S_IRWXU);
    mkfifo(src, S_IRWXU);
    // Fork the process using shell &
    gchar *cmd = mpf_param_get_string("command");
    gchar *process = g_strdup_printf(cmd, src, sink);
    gchar *bg = g_strconcat(process, " &", 0);

    MPF_LOG_DEBUG(&mpf_private.logger,"system process=%s\n", bg);
    system(bg);

    // Open the pipes.  Order is important, must match order of process to avoid deadlock.
    if (mpf_private.sink_first) {
		MPF_PRIVATE_DEBUG("opening readPipe (%s)\n", sink);
		mpf_private.readPipe = fopen(sink, "r");
		MPF_PRIVATE_DEBUG("opening writePipe (%s)\n", src);
		mpf_private.writePipe = fopen(src, "w");
    } else {
		MPF_PRIVATE_DEBUG("opening writePipe (%s)\n", src);
		mpf_private.writePipe = fopen(src, "w");
		MPF_PRIVATE_DEBUG("opening readPipe (%s)\n", sink);
		mpf_private.readPipe = fopen(sink, "r");
    }

    MPF_LOG_DEBUG(&mpf_private.logger,"readPipe=%p, writePipe=%p\n", mpf_private.readPipe, mpf_private.writePipe);

    GstPad *input = gst_element_get_pad(GST_ELEMENT(mpf_component_get_curcomponent()), "input");
    gst_pad_set_event_function(input, pad_event_handler);

    // Start a thread to consume the src
    GstPad *outputpad = gst_element_get_pad(GST_ELEMENT(component), "output");
    gst_pad_start_task(outputpad, pipe_reader, component);

}

/* The main process function is called once per "frame".
 * All the buffers at the same timestamp from each of the inputs are available.
 */
component_process() {
  /* Get the input */
  // IplImage *xxx = mpf_iplimage_pull("input_iplimage");

  /* Do any processing */
  // int count = mpf_param_get_int("object-count");
  // GrdfGraph *graph = mpf_rdf_new();
  // GrdfNode *node = grdf_node_anon_new(graph, "count");
  // grdf_stmt_new_nuu(graph, node, "isA", "AppscioScalar");
  // grdf_stmt_new_nui(graph, node, "AppscioScalarValue", count);

  /* Print a message to show we are doing something */
  mpf_private.buffer_count++;
  MPF_LOG_DEBUG(&mpf_private.logger, "%s buffer %d\n", COMPONENT_NAME, mpf_private.buffer_count);

  /* If the input image is not being sent down the pipeline then release it. */
  // mpf_iplimage_unref(xxx);

  /* Push the results out to the next component in the pipeline. */
  void *in = mpf_voidstar_pull("input");
  gint len = mpf_voidstar_get_length(in);
  MPF_LOG_DEBUG(&mpf_private.logger,"input pulled (%d)\n", len);

  if (in && mpf_private.wrap) {
      Wrapper wrapper = {0, 0, 0};
      MPF_LOG_DEBUG(&mpf_private.logger,"wrapping buffer (%d)\n", len);
      wrapper.size = len;
      wrapper.timestamp = mpf_buffer_get_timestamp(in);
      wrapper.duration = mpf_buffer_get_duration(in);
      MPF_LOG_DEBUG(&mpf_private.logger,"wrapped size=%d timestamp=%lld (0x%0llx) duration=%lld (0x%0llx)\n", wrapper.size, wrapper.timestamp, wrapper.timestamp, wrapper.duration, wrapper.duration);
      wrapper.size = htonl(wrapper.size);
      wrapper.timestamp = htobe64(wrapper.timestamp);
      wrapper.duration = htobe64(wrapper.duration);
      fwrite(&wrapper, 1, sizeof(wrapper), mpf_private.writePipe);
  }

  len = fwrite(in, 1, len, mpf_private.writePipe);
  MPF_LOG_DEBUG(&mpf_private.logger,"wrote: %d\n", len);
  fflush(mpf_private.writePipe);
  if (mpf_private.close) {
      MPF_LOG_DEBUG(&mpf_private.logger,"autoclose/reopen\n");
      fclose(mpf_private.writePipe);
      mpf_private.writePipe = fopen(mpf_private.src, "w");
  }

  return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
    remove(mpf_private.sink);
    remove(mpf_private.src);
}

static void pipe_reader(void* data) {
    MpfComponent *component = (MpfComponent*)data;
    mpf_component_set_curcomponent(component);
    GstPad *outputpad = gst_element_get_pad(&component->element, "output");

    guint size = mpf_private.size;
    if (mpf_private.wrap) {
        Wrapper wrapper = {0, 0, 0};
        fread(&wrapper, 1, sizeof(wrapper), mpf_private.readPipe);
        size = ntohl(wrapper.size);
        wrapper.timestamp = be64toh(wrapper.timestamp);
        wrapper.duration = be64toh(wrapper.duration);
        MPF_LOG_DEBUG(&mpf_private.logger,"unwrapping buffer (%d) timestamp=%lld, duration=%lld\n", size, wrapper.timestamp, wrapper.duration);
    }
    MPF_PRIVATE_DEBUG("pipe_reader reading readPipe\n");
    gchar *buf = malloc(size+1);
    guint len = fread(buf, 1, size, mpf_private.readPipe);
    if (len <= 0) {
        MPF_LOG_DEBUG(&mpf_private.logger,"pipe_reader done.\n");
        gst_task_pause(GST_PAD_TASK(outputpad));
        gst_pad_push_event(outputpad, gst_event_new_eos());
        return;
    }
    buf[size] = 0;
    MPF_LOG_DEBUG(&mpf_private.logger,"pipe_reader data (%d): %s\n", len, buf);
    void* out = mpf_voidstar_stralloc(buf);
    mpf_voidstar_set_length(out, len);
    mpf_voidstar_push("output", out);
    mpf_voidstar_send_outbuffers();

}

// Look for end of stream on input, use it to close the writePipe.
static gboolean
pad_event_handler(GstPad *pad, GstEvent *event)
{
  GstElement *element = gst_pad_get_parent_element(pad);
  MpfComponent *component = MPF_COMPONENT (GST_OBJECT_PARENT (pad));
  // If EOS, poke a message out of the events pad.
  if (event->type == GST_EVENT_EOS) {
    MPF_LOG_DEBUG(&mpf_private.logger, "EOS\n");
    // Guard multiple EOS
    if (mpf_private.writePipe) {
        MPF_LOG_DEBUG(&mpf_private.logger, "Closing writePipe %p (%s)\n", mpf_private.writePipe, mpf_private.src);
        fclose(mpf_private.writePipe);
        mpf_private.writePipe = 0;
    }
  }

  return TRUE;
}
