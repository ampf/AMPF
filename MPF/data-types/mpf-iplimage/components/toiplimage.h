/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * SECTION: mpf-opencv
 * @short_description:
 */

#ifndef __GST_TOIPLIMAGE_H__
#define __GST_TOIPLIMAGE_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_TOIPLIMAGE \
	(gst_toiplimage_get_type())
#define GST_TOIPLIMAGE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_TOIPLIMAGE,GstToIplImage))
#define GST_TOIPLIMAGE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_TOIPLIMAGE,GstToIplImageClass))
#define GST_IS_TOIPLIMAGE(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_TOIPLIMAGE))
#define GST_IS_TOIPLIMAGE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_TOIPLIMAGE))

typedef struct _GstToIplImage      GstToIplImage;
typedef struct _GstToIplImageClass GstToIplImageClass;

struct _GstToIplImage
{
  /*< private >*/
  GstElement element;

  GstPad *sinkpad, *srcpad;

  GstCaps *srccaps;

  gint depth, channels, width, height;
};

struct _GstToIplImageClass
{
  /*< private >*/
  GstElementClass parent_class;
};

GType gst_toiplimage_get_type (void);

G_END_DECLS

#endif /* __GST_TOIPLIMAGE_H__ */
