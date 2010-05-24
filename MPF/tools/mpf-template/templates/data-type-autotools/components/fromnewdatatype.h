/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * SECTION: mpf-newdatatype
 * @short_description:
 */

#ifndef __GST_FROMNEWDATATYPE_H__
#define __GST_FROMNEWDATATYPE_H__

#include <gst/gst.h>

G_BEGIN_DECLS

#define GST_TYPE_FROMNEWDATATYPE \
  (gst_fromnewdatatype_get_type())
#define GST_FROMNEWDATATYPE(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_FROMNEWDATATYPE, \
  GstFromNewdatatype))
#define GST_FROMNEWDATATYPE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_FROMNEWDATATYPE, \
  GstFromNewdatatypeClass))
#define GST_IS_FROMNEWDATATYPE(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_FROMNEWDATATYPE))
#define GST_IS_FROMNEWDATATYPE_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_FROMNEWDATATYPE))

typedef struct _GstFromNewdatatype GstFromNewdatatype;
typedef struct _GstFromNewdatatypeClass GstFromNewdatatypeClass;

struct _GstFromNewdatatype {
  /*< private >*/
  GstElement element;
  GstPad *sinkpad, *srcpad;
  GstCaps *srccaps;
  gint depth, channels, width, height;
};

struct _GstFromNewdatatypeClass {
  /*< private >*/
  GstElementClass parent_class;
};

GType gst_fromnewdatatype_get_type(void);

G_END_DECLS

#endif /* __GST_FROMNEWDATATYPE_H__ */
