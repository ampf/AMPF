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

#include <cv.h>

#include <mpf/mpf-iplimage.h>
#include "toiplimage.h"

GST_DEBUG_CATEGORY_STATIC (gst_toiplimage_debug);
#define GST_CAT_DEFAULT gst_toiplimage_debug

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

static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
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

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (
        MPF_IPLIMAGE_CAPS
    )
);

GST_BOILERPLATE (GstToIplImage, gst_toiplimage, GstElement,
    GST_TYPE_ELEMENT);

static void gst_toiplimage_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_toiplimage_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_toiplimage_set_caps (GstPad * pad, GstCaps * caps);
static GstFlowReturn gst_toiplimage_chain (GstPad * pad, GstBuffer * buf);

static void
gst_toiplimage_base_init (gpointer gclass)
{
  static GstElementDetails element_details = {
    "Convert to IplImage",
    "Generic/PluginTemplate",
    "Convert raw video frames into IplImage for OpenCV elements",
    "Erik Walthinsen <omega@appscio.com>"
  };
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_factory));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_set_details (element_class, &element_details);
}

/* initialize the plugin's class */
static void
gst_toiplimage_class_init (GstToIplImageClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_toiplimage_set_property;
  gobject_class->get_property = gst_toiplimage_get_property;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set functions
 * initialize structure
 */
static void
gst_toiplimage_init (GstToIplImage * filter,
    GstToIplImageClass * gclass)
{
  GstElementClass *klass = GST_ELEMENT_GET_CLASS (filter);

  filter->sinkpad =
      gst_pad_new_from_template (gst_element_class_get_pad_template (klass,
          "sink"), "sink");
  gst_pad_set_setcaps_function (filter->sinkpad,
                                GST_DEBUG_FUNCPTR(gst_toiplimage_set_caps));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_toiplimage_chain));

  filter->srcpad =
      gst_pad_new_from_template (gst_element_class_get_pad_template (klass,
          "src"), "src");

  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);
}

static void
gst_toiplimage_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
//  GstToIplImage *filter = GST_TOIPLIMAGE (object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_toiplimage_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
//  GstToIplImage *filter = GST_TOIPLIMAGE (object);

  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles the link with other elements */
static gboolean
gst_toiplimage_set_caps (GstPad * pad, GstCaps * caps)
{
  GstToIplImage *filter;
  GstStructure *instruct, *outstruct;
  GstCaps *tmplcaps;
  int bpp, depth;
  gboolean res;

  // This obtains a reference to the element, we must free it later
  filter = GST_TOIPLIMAGE (gst_pad_get_parent (pad));

  g_assert(caps != NULL);
  g_assert(GST_IS_CAPS(caps));

//  fprintf(stderr,"TOIPLIMAGE: in toiplimage_set_caps()\n");
//  fprintf(stderr,"TOIPLIMAGE: in caps are %s\n",gst_caps_to_string(caps));

  // Get the structure from the input side and pull what we need
  instruct = gst_caps_get_structure (caps, 0);
  if (g_str_equal(gst_structure_get_name(instruct), "video/x-raw-rgb")) {
    filter->channels = 3;
  } else if (g_str_equal(gst_structure_get_name(instruct), "video/x-raw-gray")) {
    filter->channels = 1;
  } else {
    GST_DEBUG_OBJECT (filter, "invalid MIME type!");
    return FALSE;
  }
  res = gst_structure_get_int (instruct, "width", &filter->width);
  res &= gst_structure_get_int (instruct, "height", &filter->height);
  res &= gst_structure_get_int (instruct, "depth", &depth);
  res &= gst_structure_get_int (instruct, "bpp", &bpp);
  if (!res) {
    GST_DEBUG_OBJECT (filter, "did not specify width, height, or bpp on input");
    return FALSE;
  }
  filter->depth = bpp / filter->channels;
  if ((filter->depth != 8) && (filter->depth != 16)) {
    GST_DEBUG_OBJECT (filter, "unknown image depth %d\n",filter->depth);
    return FALSE;
  }

//  fprintf(stderr,"TOIPLIMAGE: image is %dx%d, depth %d, %d channels\n",filter->width,filter->height,filter->depth,filter->channels);

  // Construct a set of caps from the template
  tmplcaps = gst_pad_get_allowed_caps (filter->srcpad);
  filter->srccaps = gst_caps_copy_nth (tmplcaps, 0);
  gst_caps_unref(tmplcaps);
  outstruct = gst_caps_get_structure (filter->srccaps, 0);

  // Set the parameters and fixate
  gst_structure_set (outstruct, "width", G_TYPE_INT, filter->width, NULL);
  gst_structure_set (outstruct, "height", G_TYPE_INT, filter->height, NULL);
  if (filter->depth == 8)
    gst_structure_set (outstruct, "format", G_TYPE_STRING, "8U", NULL);
  else
    gst_structure_set (outstruct, "format", G_TYPE_STRING, "16U", NULL);
  gst_structure_set (outstruct, "channels", G_TYPE_INT, filter->channels, NULL);

//  fprintf(stderr,"***** toiplimage: setting caps to \"%s\"\n",
//		gst_caps_to_string(filter->srccaps));

  gst_object_unref(filter);

  return TRUE;
}

/* chain function
 * this function does the actual processing
 */

static GstFlowReturn
gst_toiplimage_chain (GstPad * pad, GstBuffer * buf)
{
  GstToIplImage *filter = GST_TOIPLIMAGE (GST_OBJECT_PARENT (pad));

  GstBuffer *newbuf;
  int newbufsize;
  IplImage *newimg;

  // New buffer is IplImage and data concatenated
  newbufsize = sizeof(IplImage) + GST_BUFFER_SIZE(buf);

  // Allocate a new buffer and copy the relevant metadata across
  newbuf = gst_buffer_new_and_alloc(newbufsize);
#if GST_VERSION_NANO >= 13
  gst_buffer_copy_metadata(newbuf, buf,
	GST_BUFFER_COPY_FLAGS | GST_BUFFER_COPY_TIMESTAMPS);
#else
  {
    guint mask;

    /* copy relevant flags */
    mask = GST_BUFFER_FLAG_PREROLL | GST_BUFFER_FLAG_IN_CAPS |
        GST_BUFFER_FLAG_DELTA_UNIT | GST_BUFFER_FLAG_DISCONT |
        GST_BUFFER_FLAG_GAP;

    GST_MINI_OBJECT_FLAGS (newbuf) |= GST_MINI_OBJECT_FLAGS (buf) & mask;

    GST_BUFFER_TIMESTAMP (newbuf) = GST_BUFFER_TIMESTAMP (buf);
    GST_BUFFER_DURATION (newbuf) = GST_BUFFER_DURATION (buf);
    GST_BUFFER_OFFSET (newbuf) = GST_BUFFER_OFFSET (buf);
    GST_BUFFER_OFFSET_END (newbuf) = GST_BUFFER_OFFSET_END (buf);
  }
#endif

  // Initialize the IplImage struct
  newimg = (IplImage *)GST_BUFFER_DATA(newbuf);
  cvInitImageHeader(newimg, cvSize(filter->width,filter->height),
		    IPL_DEPTH_8U,filter->channels,0,4);

  // Make sure it's treated as a row-packed image
  newimg->widthStep = filter->width * filter->depth / 8 * filter->channels;
  newimg->imageData = ((char *)newimg + sizeof(IplImage));

  // Copy the image data itself
  memcpy(newimg->imageData, GST_BUFFER_DATA(buf), GST_BUFFER_SIZE(buf));

  gst_buffer_set_caps(newbuf, filter->srccaps);

  // Unref the old buffer
  gst_buffer_unref(buf);

//  fprintf(stderr,"** TOIPLIMAGE: sending buffer %p(%p)\n",newbuf,GST_BUFFER_DATA(newbuf));

  return gst_pad_push (filter->srcpad, newbuf);
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and pad templates
 * register the features
 *
 * exchange the string 'plugin' with your elemnt name
 */
static gboolean
plugin_init (GstPlugin * plugin)
{
  /* exchange the strings 'plugin' and 'Template plugin' with your
   * plugin name and description */
  GST_DEBUG_CATEGORY_INIT (gst_toiplimage_debug, "toiplimage",
      0, "Convert to IplImage");

  return gst_element_register (plugin, "toiplimage",
      GST_RANK_NONE, GST_TYPE_TOIPLIMAGE);
}

/* this is the structure that gstreamer looks for to register plugins
 *
 * exchange the strings 'plugin' and 'Template plugin' with you plugin name and
 * description
 */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "toiplimage",
    "Convert to IplImage",
    plugin_init, VERSION, "LGPL", "GStreamer", "http://gstreamer.net/")
