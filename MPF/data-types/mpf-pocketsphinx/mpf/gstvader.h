/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef __GST_VADER_H__
#define __GST_VADER_H__


#include <gst/gst.h>
#include <gst/gstevent.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */
#if 0
}
#endif

#define GST_TYPE_VADER				\
    (gst_vader_get_type())
#define GST_VADER(obj)                                          \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_VADER,GstVader))
#define GST_VADER_CLASS(klass)						\
    (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_VADER,GstVaderClass))
#define GST_IS_VADER(obj)                               \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_VADER))
#define GST_IS_VADER_CLASS(klass)                       \
    (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_VADER))

/* Custom events inserted in the stream at start and stop of cuts. */
#define GST_EVENT_VADER_START						\
    GST_EVENT_MAKE_TYPE(146, GST_EVENT_TYPE_DOWNSTREAM | GST_EVENT_TYPE_SERIALIZED)
#define GST_EVENT_VADER_STOP						\
    GST_EVENT_MAKE_TYPE(147, GST_EVENT_TYPE_DOWNSTREAM | GST_EVENT_TYPE_SERIALIZED)

typedef struct _GstVader GstVader;
typedef struct _GstVaderClass GstVaderClass;

/* Maximum frame size over which VAD is calculated. */
#define VADER_FRAME 512
/* Number of frames over which to vote on speech/non-speech decision. */
#define VADER_WINDOW 5

struct _GstVader
{
    GstElement element;

    GstPad *sinkpad, *srcpad;

    GStaticRecMutex mtx;          /**< Lock used in setting parameters. */

    gboolean window[VADER_WINDOW];/**< Voting window of speech/silence decisions. */
    gboolean silent;		  /**< Current state of the filter. */
    gboolean silent_prev;	  /**< Previous state of the filter. */
    GList *pre_buffer;            /**< list of GstBuffers in pre-record buffer */
    guint64 silent_run_length;    /**< How much silence have we endured so far? */
    guint64 pre_run_length;       /**< How much pre-silence have we endured so far? */

    gint threshold_level;         /**< Silence threshold level (Q15, adaptive). */
    guint64 threshold_length;     /**< Minimum silence for cutting, in nanoseconds. */
    guint64 pre_length;           /**< Pre-buffer to add on silence->speech transition. */

    gboolean auto_threshold;      /**< Set threshold automatically. */
    gint silence_mean;            /**< Mean RMS power of silence frames. */
    gint silence_stddev;          /**< Variance in RMS power of silence frames. */
    gint silence_frames;          /**< Number of frames used in estimating mean/variance */

    gchar *dumpdir;               /**< Directory to dump audio to (for debugging). */
    FILE *dumpfile;		  /**< Current audio dump file. */
    gint dumpidx;                 /**< Dump file index. */
};

struct _GstVaderClass
{
    GstElementClass parent_class;
    void (*vader_start) (GstVader* filter);
    void (*vader_stop) (GstVader* filter);
};

GType gst_vader_get_type (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __GST_STEREO_H__ */
