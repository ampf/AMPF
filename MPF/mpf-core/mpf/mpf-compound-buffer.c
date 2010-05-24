/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <mpf/mpf-compound-buffer.h>

static void mpf_compound_buffer_class_init (gpointer g_class, gpointer class_data);
static void mpf_compound_buffer_init (GTypeInstance *instance, gpointer g_class);
static void mpf_compound_buffer_finalize (MpfCompoundBuffer *buffer);
static GstBuffer *mpf_compound_buffer_copy (MpfCompoundBuffer *buffer);

static GstBufferClass *parent_class;

GType
mpf_compound_buffer_get_type (void)
{
  static GType _mpf_compound_buffer_type;

  if (G_UNLIKELY (_mpf_compound_buffer_type == 0)) {
    static const GTypeInfo compound_buffer_info = {
      sizeof (GstBufferClass),
      NULL,
      NULL,
      mpf_compound_buffer_class_init,
      NULL,
      NULL,
      sizeof (MpfCompoundBuffer),
      0,
      (GInstanceInitFunc) mpf_compound_buffer_init,
      NULL
    };
    _mpf_compound_buffer_type = g_type_register_static (GST_TYPE_BUFFER,
        "MpfCompoundBuffer", &compound_buffer_info, 0);
  }
  return _mpf_compound_buffer_type;
}


static void
mpf_compound_buffer_class_init (gpointer g_class, gpointer class_data)
{
  GstMiniObjectClass *mini_object_class = GST_MINI_OBJECT_CLASS (g_class);

  parent_class = g_type_class_peek_parent (g_class);

  mini_object_class->finalize =
      (GstMiniObjectFinalizeFunction) mpf_compound_buffer_finalize;
  mini_object_class->copy =
      (GstMiniObjectCopyFunction) mpf_compound_buffer_copy;
}

static void
mpf_compound_buffer_init (GTypeInstance * instance, gpointer g_class)
{
}

static void
mpf_compound_buffer_finalize (MpfCompoundBuffer *compound_buffer)
{
  (compound_buffer->finalize) (compound_buffer);
}

static GstBuffer *
mpf_compound_buffer_copy (MpfCompoundBuffer *compound_buffer)
{
  return (GstBuffer *)((compound_buffer->copy) (compound_buffer));
}

GstBuffer *
mpf_compound_buffer_new (void *data, int size,
    MpfCompoundBufferFinalizeFunc finalize, MpfCompoundBufferCopyFunc copy,
    void *priv)
{
  GstBuffer *buf;
  MpfCompoundBuffer *mpfbuf;

  buf = (GstBuffer *) gst_mini_object_new (mpf_compound_buffer_get_type());
  mpfbuf = (MpfCompoundBuffer *)buf;

  buf->data = data;
  buf->size = size;
  mpfbuf->finalize = finalize;
  mpfbuf->copy = copy;
  mpfbuf->priv = priv;

  return buf;
}

