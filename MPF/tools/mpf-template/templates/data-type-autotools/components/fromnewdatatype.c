/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * SECTION:element-plugin
 *
 * <refsect2>
 * <title>Example launch line</title>
 * <para>
 * <programlisting>
 * gst-launch -v -m audiotestsrc ! plugin ! fakesink silent=TRUE
 * </programlisting>
 * </para>
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/video.h>

/* Include your specific data type include files here */
//#include <newdatatype.h>

#include <mpf/mpf-newdatatype.h>
#include "fromnewdatatype.h"

GST_DEBUG_CATEGORY_STATIC (gst_fromnewdatatype_debug);
#define GST_CAT_DEFAULT gst_fromnewdatatype_debug

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  ARG_0,
};

/* Fill in the static caps as appropriate for your data-type */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
	"video/x-raw-newdatatype, "
	"format = (string) 8U, "
	"channels = (int) { 1, 3 }, "
	"width = " GST_VIDEO_SIZE_RANGE ", "
	"height = " GST_VIDEO_SIZE_RANGE ";"

	"video/x-raw-newdatatype, "
	"format = (string) 16U, "
	"channels = (int) 1, "
	"width = " GST_VIDEO_SIZE_RANGE ", "
	"height = " GST_VIDEO_SIZE_RANGE
    )
);

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
	GST_VIDEO_CAPS_BGR ";"

	"video/x-raw-gray, "
	"bpp = (int) 8, "
	"depth = (int) 8, "
	"width = " GST_VIDEO_SIZE_RANGE ", "
	"height = " GST_VIDEO_SIZE_RANGE ", "
	"framerate = " GST_VIDEO_FPS_RANGE ";"

	"video/x-raw-gray, "
	"bpp = (int) 16, "
	"depth = (int) 16, "
	"width = " GST_VIDEO_SIZE_RANGE ", "
	"height = " GST_VIDEO_SIZE_RANGE ", "
	"framerate = " GST_VIDEO_FPS_RANGE
    )
);

GST_BOILERPLATE (GstFromNewdatatype, gst_fromnewdatatype, GstElement,
    GST_TYPE_ELEMENT);

static void gst_fromnewdatatype_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_fromnewdatatype_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_fromnewdatatype_set_caps (GstPad * pad, GstCaps * caps);
static GstFlowReturn gst_fromnewdatatype_chain (GstPad * pad, GstBuffer * buf);

static void gst_fromnewdatatype_base_init(gpointer gclass) {
  static GstElementDetails element_details = {
    "Convert from Newdatatype",
    "Generic/PluginTemplate",
    "Convert Newdatatype into raw frames",
    "Appscio, Inc. <info@appscio.com>"
  };
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_add_pad_template(element_class,
      gst_static_pad_template_get(&src_factory));
  gst_element_class_add_pad_template(element_class,
      gst_static_pad_template_get(&sink_factory));
  gst_element_class_set_details(element_class, &element_details);
}

/* initialize the plugin's class */
static void gst_fromnewdatatype_class_init(GstFromNewdatatypeClass * klass) {
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_fromnewdatatype_set_property;
  gobject_class->get_property = gst_fromnewdatatype_get_property;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set functions
 * initialize structure
 */
static void gst_fromnewdatatype_init(GstFromNewdatatype * filter,
    GstFromNewdatatypeClass * gclass) {

  GstElementClass *klass = GST_ELEMENT_GET_CLASS(filter);

  filter->sinkpad =
      gst_pad_new_from_template(gst_element_class_get_pad_template(klass,
      "sink"), "sink");
  gst_pad_set_setcaps_function(filter->sinkpad,
      GST_DEBUG_FUNCPTR(gst_fromnewdatatype_set_caps));
  gst_pad_set_chain_function(filter->sinkpad,
      GST_DEBUG_FUNCPTR(gst_fromnewdatatype_chain));

  filter->srcpad =
      gst_pad_new_from_template(gst_element_class_get_pad_template(klass,
      "src"), "src");

  gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);
  gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);
}

static void gst_fromnewdatatype_set_property(GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec) {

//  GstFromNewdatatype *filter = GST_FROMNEWDATATYPE(object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

static void gst_fromnewdatatype_get_property(GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec) {

//  GstFromNewdatatype *filter = GST_FROMNEWDATATYPE(object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles the link with other elements */
static gboolean gst_fromnewdatatype_set_caps(GstPad * pad, GstCaps * caps) {
  GstFromNewdatatype *filter;
  GstStructure *instruct, *outstruct;
  GstCaps *tmplcaps;
  const gchar *format;
  int depth, channels;
  int width, height;

  // This obtains a reference to the element, we must free it later
  filter = GST_FROMNEWDATATYPE(gst_pad_get_parent(pad));

//  fprintf(stderr,"FROMNEWDATATYPE: in fromnewdatatype_set_caps()\n");
//  fprintf(stderr,"FROMNEWDATATYPE: caps are %s\n",gst_caps_to_string(caps));

  fprintf(stderr,"***** fromnewdatatype: input caps are \"%s\"\n",
      gst_caps_to_string(caps));

  // Get the structure from the input side and pull what we need
  instruct = gst_caps_get_structure(caps, 0);
  format = gst_structure_get_string(instruct, "format");
  gst_structure_get_int(instruct, "width", &width);
  gst_structure_get_int(instruct, "height", &height);
  gst_structure_get_int(instruct, "channels", &channels);

  if (g_str_equal(format, "8U"))
    depth = 8;
  else if (g_str_equal(format, "16U"))
    depth = 16;
  else {
    GST_DEBUG_OBJECT(filter, "unknown image format %s\n", format);
    return FALSE;
  }

//  fprintf(stderr,"FROMNEWDATATYPE: image is %dx%d, depth %d, %d channels\n", width, height, depth, channels);

  // Construct a set of caps from the template
  tmplcaps = gst_pad_get_allowed_caps(filter->srcpad);

  if (channels == 3)
    filter->srccaps = gst_caps_copy_nth(tmplcaps, 0);
  else if (channels == 1) {
    if (depth == 8)
      filter->srccaps = gst_caps_copy_nth(tmplcaps, 1);
    else if (depth == 16)
      filter->srccaps = gst_caps_copy_nth(tmplcaps, 2);
    else {
      GST_DEBUG_OBJECT(filter, "unknown image depth %d\n", depth);
      return FALSE;
    }
  } else {
    GST_DEBUG_OBJECT(filter, "unknown channel count %d\n", channels);
    return FALSE;
  }

  gst_caps_unref(tmplcaps);
  outstruct = gst_caps_get_structure(filter->srccaps, 0);

  // Set the parameters and fixate
  gst_structure_set(outstruct, "width", G_TYPE_INT, width, NULL);
  gst_structure_set(outstruct, "height", G_TYPE_INT, height, NULL);
  gst_structure_set(outstruct, "framerate", GST_TYPE_FRACTION, 30, 1, NULL);

//  fprintf(stderr, "FROMNEWDATATYPE: out caps are %s\n", gst_caps_to_string(filter->srccaps));

  gst_object_unref(filter);

  return TRUE;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn gst_fromnewdatatype_chain(GstPad * pad, GstBuffer * buf) {
  GstFromNewdatatype *filter = GST_FROMNEWDATATYPE(GST_OBJECT_PARENT(pad));
  GstBuffer *newbuf;

  newbuf = gst_buffer_create_sub(buf, sizeof(Newdatatype),
      GST_BUFFER_SIZE(buf) - sizeof(Newdatatype));

//  fprintf(stderr, "** FROMNEWDATATYPE: have image %p(%p) t=%lld\n", buf, GST_BUFFER_DATA(buf), GST_BUFFER_TIMESTAMP(buf));

  // Unref the old buffer
  gst_buffer_unref(buf);

  gst_buffer_set_caps(newbuf, filter->srccaps);

  return gst_pad_push(filter->srcpad, newbuf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and pad templates
 * register the features
 */
static gboolean plugin_init(GstPlugin * plugin) {
  GST_DEBUG_CATEGORY_INIT(gst_fromnewdatatype_debug, "fromnewdatatype",
      0, "Convert to Newdatatype");

  return gst_element_register(plugin, "fromnewdatatype",
      GST_RANK_NONE, GST_TYPE_FROMNEWDATATYPE);
}

/* this is the structure that gstreamer looks for to register plugins
 */
GST_PLUGIN_DEFINE(GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "fromnewdatatype",
    "Convert to Newdatatype",
    plugin_init, VERSION, "LGPL", "GStreamer", "http://gstreamer.net/")

