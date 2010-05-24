/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdint.h>

#include <mpf/mpf-component.h>

#define MPF_UTTERANCE_CAPS		\
	"application/x-mpf-utterance"

/* --- typedefs & structures --- */
typedef struct _MpfUtterance MpfUtterance;

/**
 * MpfUtterance:
 *
 * All the fields in the <structname>MpfUtterance</structname> structure are private
 * to the #MpfUtterance implementation and should never be accessed directly.
 */
struct _MpfUtterance {
  guint32 id;
  gint32 score;
  gint textlen;
  gboolean final;
  gchar *text;
};

GstStaticCaps mpf_utterance_caps;

/* --- prototypes --- */
void mpf_utterance_add_input (const gchar *name);
void mpf_utterance_add_output (const gchar *name);

#define mpf_utterance_pull(padname) ((MpfUtterance *)mpf_component_pull(NULL,padname))
void mpf_utterance_push (const gchar *padname, MpfUtterance *utterance);

MpfUtterance *mpf_utterance_new (guint32 id,gint32 score,gchar *text);
MpfUtterance *mpf_utterance_serialize (MpfUtterance *utterance);

#define mpf_utterance_ref(utterance) mpf_component_data_ref(utterance);
#define mpf_utterance_unref(utterance) mpf_component_data_unref(utterance);

MpfUtterance *mpf_utterance_make_writable(MpfUtterance *utterance);

