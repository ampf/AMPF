/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * SECTION: mpf-opencv
 * @short_description:
 */

#ifndef __GST_FROMIPLIMAGE_H__
#define __GST_FROMIPLIMAGE_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_FROMIPLIMAGE \
  (gst_fromiplimage_get_type())
#define GST_FROMIPLIMAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_FROMIPLIMAGE,GstFromIplImage))
#define GST_FROMIPLIMAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_FROMIPLIMAGE,GstFromIplImageClass))
#define GST_IS_FROMIPLIMAGE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_FROMIPLIMAGE))
#define GST_IS_FROMIPLIMAGE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_FROMIPLIMAGE))

typedef struct _GstFromIplImage      GstFromIplImage;
typedef struct _GstFromIplImageClass GstFromIplImageClass;

struct _GstFromIplImage
{
  /*< private >*/
  GstElement element;

  GstPad *sinkpad, *srcpad;

  GstCaps *srccaps;

  gint depth, channels, width, height;
};

struct _GstFromIplImageClass
{
  /*< private >*/
  GstElementClass parent_class;
};

GType gst_fromiplimage_get_type (void);

G_END_DECLS

#endif /* __GST_FROMIPLIMAGE_H__ */
