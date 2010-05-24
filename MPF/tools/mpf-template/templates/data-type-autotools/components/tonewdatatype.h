/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * SECTION: mpf-newdatatype
 * @short_description:
 */

#ifndef __GST_TONEWDATATYPE_H__
#define __GST_TONEWDATATYPE_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_TONEWDATATYPE \
	(gst_tonewdatatype_get_type())
#define GST_TONEWDATATYPE(obj) \
	(G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_TONEWDATATYPE,GstToNewdatatype))
#define GST_TONEWDATATYPE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_TONEWDATATYPE,GstToNewdatatypeClass))
#define GST_IS_TONEWDATATYPE(obj) \
	(G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_TONEWDATATYPE))
#define GST_IS_TONEWDATATYPE_CLASS(klass) \
	(G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_TONEWDATATYPE))

typedef struct _GstToNewdatatype      GstToNewdatatype;
typedef struct _GstToNewdatatypeClass GstToNewdatatypeClass;

struct _GstToNewdatatype
{
  /*< private >*/
  GstElement element;
  GstPad *sinkpad, *srcpad;
  GstCaps *srccaps;
  gint depth, channels, width, height;
};

struct _GstToNewdatatypeClass
{
  /*< private >*/
  GstElementClass parent_class;
};

GType gst_tonewdatatype_get_type (void);

G_END_DECLS

#endif /* __GST_TONEWDATATYPE_H__ */
