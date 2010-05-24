/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <gst/gst.h>
#include "gstiplimage.h"
/**
 * SECTION: mpf-opencv
 * @short_description:
 */
#include <string.h>

static void
gst_iplimage_buffer_init (GstIplImageBuffer * iplimage, gpointer g_class)
{
}

static GstIplImageBuffer *
gst_iplimage_buffer_copy (GstIplImageBuffer * iplimage)
{
  GstIplImageBuffer *copy;

  g_return_val_if_fail (iplimage != NULL, NULL);

  /* Allocate a new IplImage buffer */
  copy = (GstIplImageBuffer *) gst_mini_object_new (GST_TYPE_IPLIMAGE_BUFFER);

  /* Copy the IplImage itself into place */
  memcpy (GST_BUFFER_DATA(copy), GST_BUFFER_DATA(iplimage), GST_BUFFER_SIZE(iplimage));

  /* Copy the image data buffer as well */
  copy->imagedata = gst_buffer_copy (iplimage->imagedata);

  return copy;
}

static void
gst_iplimage_buffer_finalize (GstIplImageBuffer * iplimage)
{
  if (iplimage->imagedata != NULL) {
    gst_buffer_unref (iplimage->imagedata);
  }
  return;
}

static void
gst_iplimage_buffer_class_init (gpointer g_class, gpointer class_data)
{
  GstMiniObjectClass *mini_object_class = GST_MINI_OBJECT_CLASS (g_class);

//  iplimage_buffer_parent_class = g_type_class_peek_parent (g_class);

  mini_object_class->copy = (GstMiniObjectCopyFunction)
      gst_iplimage_buffer_copy;
  mini_object_class->finalize = (GstMiniObjectFinalizeFunction)
      gst_iplimage_buffer_finalize;
}

/**
 * gst_iplimage_buffer_get_type:
 *
 * Returns the type for a #GstIplImageBuffer.
 */
GType
gst_iplimage_buffer_get_type (void)
{
  static GType _gst_iplimage_buffer_type;

  if (G_UNLIKELY (_gst_iplimage_buffer_type == 0)) {
    static const GTypeInfo iplimage_buffer_info = {
      sizeof (GstBufferClass),
      NULL,
      NULL,
      gst_iplimage_buffer_class_init,
      NULL,
      NULL,
      sizeof (GstIplImageBuffer),
      0,
      (GInstanceInitFunc) gst_iplimage_buffer_init,
      NULL
    };
    _gst_iplimage_buffer_type = g_type_register_static (GST_TYPE_BUFFER,
        "GstIplImageBuffer", &iplimage_buffer_info, 0);
  }
  return _gst_iplimage_buffer_type;
}

