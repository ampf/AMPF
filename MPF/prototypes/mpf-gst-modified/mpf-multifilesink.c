/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* GStreamer
 * Copyright (C) 1999,2000 Erik Walthinsen <omega@cse.ogi.edu>
 *                    2000 Wim Taymans <wtay@chello.be>
 *                    2006 Wim Taymans <wim@fluendo.com>
 *                    2006 David A. Schleef <ds@schleef.org>
 *
 * gstmultifilesink.c:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-multifilesink
 * @short_description: Writes buffers to sequentially-named files
 * @see_also: #GstFileSrc
 *
 * <para>
 * Write incoming data to a series of sequentially-named files.
 * </para>
 *
 */

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include "mpf-multifilesink.h"

static GstStaticPadTemplate sinktemplate = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS_ANY);

GST_DEBUG_CATEGORY_STATIC (mpf_multi_file_sink_debug);
#define GST_CAT_DEFAULT mpf_multi_file_sink_debug

static const GstElementDetails mpf_multi_file_sink_details =
GST_ELEMENT_DETAILS ("Multi-File Sink (MPF version)",
    "Sink/File",
    "Write buffers to a sequentially named set of files",
    "David Schleef <ds@schleef.org>, Erik Walthinsen <omega@appscio.com>");

enum
{
  ARG_0,
  ARG_LOCATION,
  ARG_INDEX
};

#define DEFAULT_LOCATION "%014lld"
#define DEFAULT_INDEX 0

static void mpf_multi_file_sink_dispose (GObject * object);

static void mpf_multi_file_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void mpf_multi_file_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstFlowReturn mpf_multi_file_sink_render (GstBaseSink * sink,
    GstBuffer * buffer);



GST_BOILERPLATE (MpfMultiFileSink, mpf_multi_file_sink, GstBaseSink,
    GST_TYPE_BASE_SINK);

static void
mpf_multi_file_sink_base_init (gpointer g_class)
{
  GstElementClass *gstelement_class = GST_ELEMENT_CLASS (g_class);

  GST_DEBUG_CATEGORY_INIT (mpf_multi_file_sink_debug, "mpf-multifilesink", 0,
      "mpf-multifilesink element");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&sinktemplate));
  gst_element_class_set_details (gstelement_class,
      &mpf_multi_file_sink_details);
}

static void
mpf_multi_file_sink_class_init (MpfMultiFileSinkClass * klass)
{
  GObjectClass *gobject_class = G_OBJECT_CLASS (klass);
  GstBaseSinkClass *gstbasesink_class = GST_BASE_SINK_CLASS (klass);

  gobject_class->set_property = mpf_multi_file_sink_set_property;
  gobject_class->get_property = mpf_multi_file_sink_get_property;

  g_object_class_install_property (gobject_class, ARG_LOCATION,
      g_param_spec_string ("location", "File Location",
          "Location of the file to write", NULL, G_PARAM_READWRITE));
  g_object_class_install_property (gobject_class, ARG_INDEX,
      g_param_spec_int ("index", "Index",
          "Index to use with location property to create file names.  The "
          "index is incremented by one for each buffer read.",
          0, INT_MAX, DEFAULT_INDEX, G_PARAM_READWRITE));

  gobject_class->dispose = mpf_multi_file_sink_dispose;

  gstbasesink_class->get_times = NULL;
  gstbasesink_class->render = GST_DEBUG_FUNCPTR (mpf_multi_file_sink_render);
}

static void
mpf_multi_file_sink_init (MpfMultiFileSink * multifilesink,
    MpfMultiFileSinkClass * g_class)
{
  GstPad *pad;

  pad = GST_BASE_SINK_PAD (multifilesink);

  multifilesink->filename = g_strdup (DEFAULT_LOCATION);
  multifilesink->index = DEFAULT_INDEX;

  GST_BASE_SINK (multifilesink)->sync = FALSE;
}

static void
mpf_multi_file_sink_dispose (GObject * object)
{
  MpfMultiFileSink *sink = MPF_MULTI_FILE_SINK (object);

  g_free (sink->filename);
  sink->filename = NULL;

  G_OBJECT_CLASS (parent_class)->dispose (object);
}

static gboolean
mpf_multi_file_sink_set_location (MpfMultiFileSink * sink,
    const gchar * location)
{
  g_free (sink->filename);
  if (location != NULL) {
    /* FIXME: validate location to have just one %d */
    sink->filename = g_strdup (location);
  } else {
    sink->filename = NULL;
  }

  return TRUE;
}
static void
mpf_multi_file_sink_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  MpfMultiFileSink *sink = MPF_MULTI_FILE_SINK (object);

  switch (prop_id) {
    case ARG_LOCATION:
      mpf_multi_file_sink_set_location (sink, g_value_get_string (value));
      break;
    case ARG_INDEX:
      sink->index = g_value_get_int (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
mpf_multi_file_sink_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  MpfMultiFileSink *sink = MPF_MULTI_FILE_SINK (object);

  switch (prop_id) {
    case ARG_LOCATION:
      g_value_set_string (value, sink->filename);
      break;
    case ARG_INDEX:
      g_value_set_int (value, sink->index);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gchar *
mpf_multi_file_sink_get_filename (MpfMultiFileSink * multifilesink)
{
  gchar *filename;

  filename = g_strdup_printf (multifilesink->filename, multifilesink->index);

  return filename;
}

static GstFlowReturn
mpf_multi_file_sink_render (GstBaseSink * sink, GstBuffer * buffer)
{
  MpfMultiFileSink *multifilesink;
  guint size;
  gchar *filename;
  gboolean ret;
  GError *error = NULL;

  size = GST_BUFFER_SIZE (buffer);

  multifilesink = MPF_MULTI_FILE_SINK (sink);

//  filename = mpf_multi_file_sink_get_filename (multifilesink);
  filename = g_strdup_printf(multifilesink->filename, GST_BUFFER_TIMESTAMP(buffer));

  ret = g_file_set_contents (filename, (char *) GST_BUFFER_DATA (buffer),
      size, &error);
  if (ret) {
    multifilesink->index++;
    g_free (filename);
    return GST_FLOW_OK;
  }

  switch (error->code) {
    case G_FILE_ERROR_NOSPC:{
      GST_ELEMENT_ERROR (multifilesink, RESOURCE, NO_SPACE_LEFT, (NULL),
          (NULL));
      break;
    }
    default:{
      GST_ELEMENT_ERROR (multifilesink, RESOURCE, WRITE,
          ("Error while writing to file \"%s\".", filename),
          ("%s", g_strerror (errno)));
    }
  }
  g_error_free (error);
  g_free (filename);

  return GST_FLOW_ERROR;
}
