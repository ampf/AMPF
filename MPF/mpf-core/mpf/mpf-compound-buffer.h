/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef _MPF_COMPOUND_BUFFER_H_
#define _MPF_COMPOUND_BUFFER_H_

#include <gst/gst.h>

G_BEGIN_DECLS

#define MPF_TYPE_COMPOUND_BUFFER \
  (mpf_compound_buffer_get_type())
#define MPF_COMPOUND_BUFFER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),MPF_TYPE_COMPOUND_BUFFER,MpfCompoundBuffer))
#define MPF_COMPOUND_BUFFER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),MPF_TYPE_COMPOUND_BUFFER,MpfCompoundBufferClass))
#define MPF_IS_COMPOUND_BUFFER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),MPF_TYPE_COMPOUND_BUFFER))
#define MPF_IS_COMPOUND_BUFFER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),MPF_TYPE_COMPOUND_BUFFER))


typedef struct _MpfCompoundBuffer MpfCompoundBuffer;
typedef struct _MpfCompoundBufferClass MpfCompoundBufferClass;
typedef void (*MpfCompoundBufferFinalizeFunc) (MpfCompoundBuffer *buf);
typedef MpfCompoundBuffer* (*MpfCompoundBufferCopyFunc) (const MpfCompoundBuffer *buf);

struct _MpfCompoundBuffer
{
  GstBuffer buffer;

  /*< private >*/
  MpfCompoundBufferFinalizeFunc finalize;
  MpfCompoundBufferCopyFunc copy;
  void *priv;
};

struct _MpfCompoundBufferClass
{
  GstBufferClass buffer_class;
};

GType mpf_compound_buffer_get_type(void);

GstBuffer *mpf_compound_buffer_new (void *data, int length,
    MpfCompoundBufferFinalizeFunc finalize, MpfCompoundBufferCopyFunc copy,
    void *priv);

G_END_DECLS

#endif
