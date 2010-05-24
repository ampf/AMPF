/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* GStreamer IplImage wrapper
 * Copyright (C) <2005> Julien Moutte <julien@moutte.net>
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

#ifndef __GST_IPLIMAGE_H__
#define __GST_IPLIMAGE_H__
/**
 * SECTION: mpf-opencv
 * @short_description:
 */
G_BEGIN_DECLS

/* --- type macros --- */
/**
 * GST_TYPE_IPLIMAGE_BUFFER:
 *
 * The type for #IplImageBuffer.
 */
#define GST_TYPE_IPLIMAGE_BUFFER (gst_iplimage_buffer_get_type())
/**
 * GST_IS_IPLIMAGE_BUFFER:
 * @obj: Instance to check for being a %GST_TYPE_IPLIMAGE_BUFFER.
 *
 * Checks whether a valid #GTypeInstance pointer is of type %GST_TYPE_IPLIMAGE_BUFFER.
 */
#define GST_IS_IPLIMAGE_BUFFER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_IPLIMAGE_BUFFER))
/**
 * GST_IPLIMAGE_BUFFER:
 * @obj: Object which is subject to casting.
 *
 * Casts a #GstIplImageBuffer or derived pointer into a (GstIplImageBuffer*) pointer.
 * Depending on the current debugging level, this function may invoke certain runtime
 * checks to identify invalid casts.
 */
#define GST_IPLIMAGE_BUFFER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_IPLIMAGE_BUFFER, GstIplImageBuffer))

/* --- typedefs and structures --- */
typedef struct _GstIplImageBuffer GstIplImageBuffer;
typedef struct _GstIplImageBufferClass GstIplImageBufferClass;

/**
 * GstIplImageBuffer:
 *
 * All the fields in the <structname>GstIplImageBuffer</structname> structure are private
 * to the #GstIplImageBuffer implementation and should never be accessed directly.
 */
struct _GstIplImageBuffer {
  GstBuffer buffer;

  guint flags;		// Needed? or use allocated bits in miniobject?
  GstBuffer *imagedata;
};

/* --- prototypes --- */
GType gst_iplimage_buffer_get_type (void);

G_END_DECLS

#endif /* __GST_IPLIMAGE_H__ */
