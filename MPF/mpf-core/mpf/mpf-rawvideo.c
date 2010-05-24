/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/video.h>

/**
 * SECTION: mpf-rawvideo
 * @short_description:
 */
#include "mpf-rawvideo.h"


typedef struct _Mpf_RawVideo_PadInfo Mpf_RawVideo_PadInfo;
struct _Mpf_RawVideo_PadInfo {
  GstCaps *padcaps;
};


gchar *
_mpf_rawvideo_get_format_caps (MpfVideoFormat format,
	gint minwidth,gint maxwidth,gint minheight,gint maxheight)
{
  gchar *caps;

  switch (format) {
    case MPF_RAWVIDEO_RGB24:
      caps = g_strdup_printf(
	"video/x-raw-rgb, "
	"bpp = (int) 24, "
	"depth = (int) 24, "
	"endianness = (int) BIG_ENDIAN, "
	"red_mask = (int) " GST_VIDEO_BYTE1_MASK_24 ", "
	"green_mask = (int) " GST_VIDEO_BYTE2_MASK_24 ", "
	"blue_mask = (int) " GST_VIDEO_BYTE3_MASK_24 ", "
	"width = (int) [ %d, %d ], "
	"height = (int) [ %d, %d ], "
	"framerate = " GST_VIDEO_FPS_RANGE,
	minwidth,maxwidth,minheight,maxheight);
      break;

    case MPF_RAWVIDEO_BGR24:
      caps = g_strdup_printf(
	"video/x-raw-rgb, "
	"bpp = (int) 24, "
	"depth = (int) 24, "
	"endianness = (int) BIG_ENDIAN, "
	"red_mask = (int) " GST_VIDEO_BYTE3_MASK_24 ", "
	"green_mask = (int) " GST_VIDEO_BYTE2_MASK_24 ", "
	"blue_mask = (int) " GST_VIDEO_BYTE1_MASK_24 ", "
	"width = (int) [ %d, %d ], "
	"height = (int) [ %d, %d ], "
	"framerate = " GST_VIDEO_FPS_RANGE,
	minwidth,maxwidth,minheight,maxheight);
      break;
  
    case MPF_RAWVIDEO_RGB32:
      caps = g_strdup_printf(
	"video/x-raw-rgb, "
	"bpp = (int) 32, "
	"depth = (int) 24, "
	"endianness = (int) BIG_ENDIAN, "
	"red_mask = (int) " GST_VIDEO_BYTE1_MASK_24 ", "
	"green_mask = (int) " GST_VIDEO_BYTE2_MASK_24 ", "
	"blue_mask = (int) " GST_VIDEO_BYTE3_MASK_24 ", "
	"width = (int) [ %d, %d ], "
	"height = (int) [ %d, %d ], "
	"framerate = " GST_VIDEO_FPS_RANGE,
	minwidth,maxwidth,minheight,maxheight);
      break;

    case MPF_RAWVIDEO_BGR32:
      caps = g_strdup_printf(
	"video/x-raw-rgb, "
	"bpp = (int) 32, "
	"depth = (int) 24, "
	"endianness = (int) BIG_ENDIAN, "
	"red_mask = (int) " GST_VIDEO_BYTE3_MASK_32 ", "
	"green_mask = (int) " GST_VIDEO_BYTE2_MASK_32 ", "
	"blue_mask = (int) " GST_VIDEO_BYTE1_MASK_32 ", "
	"width = (int) [ %d, %d ], "
	"height = (int) [ %d, %d ], "
	"framerate = " GST_VIDEO_FPS_RANGE,
	minwidth,maxwidth,minheight,maxheight);
      break;

    case MPF_RAWVIDEO_I420:
      caps = g_strdup_printf(
	"video/x-raw-yuv, "
	"format = (fourcc) I420, "
	"width = (int) [ %d, %d ], "
	"height = (int) [ %d, %d ], "
	"framerate = " GST_VIDEO_FPS_RANGE,
	minwidth,maxwidth,minheight,maxheight);
      break;

    default:
      return NULL;
      break;
  }

  return caps;
}

void
mpf_rawvideo_add_input_with_format (const gchar *padname, MpfVideoFormat
	format,gint minwidth,gint maxwidth,gint minheight,gint maxheight)
{
  gchar *caps;

  caps = _mpf_rawvideo_get_format_caps(format,minwidth,maxwidth,minheight,maxheight);
  g_assert(caps);

  mpf_rawvideo_add_caps(padname, caps);

  g_free(caps);
}

void
mpf_rawvideo_add_output_with_format (const gchar *padname, MpfVideoFormat
	format,gint minwidth,gint maxwidth,gint minheight,gint maxheight)
{
  gchar *caps;

  caps = _mpf_rawvideo_get_format_caps(format,minwidth,maxwidth,minheight,maxheight);
  g_assert(caps);

  mpf_rawvideo_remove_caps(padname, caps);

  g_free(caps);
}


/*
 * mpf_rawvideo_get_buffer:
 * @component: a #MpfComponent to find the image in (or NULL for current)
 * @data: the pointer to find the associated buffer for
 *
 * Returns a #GstBuffer that corresponds to the pointer passed to the
 * function.  This buffer either already exists and is returned as is,
 * or is created on demand and registered with the component appropriately.
 *
 * Returns: the #GstBuffer associated with the pointer
 */
GstBuffer *
mpf_rawvideo_get_buffer(MpfComponent *component, void *data)
{
  GstBuffer *buf;

  if (component == NULL)
    component = mpf_component_get_curcomponent();

  // If the buffer already exists for this data, return it
  if ((buf = mpf_component_find_buffer(component,data)))
    return buf;

  // Otherwise we need to make a new one
  buf = gst_buffer_new();
  gst_buffer_set_data(buf,(guint8 *)data,4);

  // Add it to the component's private list
  mpf_component_list_buffer(component, buf);

  return buf;
}
