/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>
#include <gst/gst.h>

#include <mpf/mpf-component.h>

#include "mpf-utterance.h"

GstStaticCaps mpf_utterance_caps = GST_STATIC_CAPS ( MPF_UTTERANCE_CAPS );

/**
 * mpf_utterance_add_input:
 * @name: named template pad.
 *
 * Adds a named template input pad to class of the current #MpfComponent.
 */
void mpf_utterance_add_input (const gchar *name) {
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;

  template = gst_pad_template_new(name, GST_PAD_SINK, GST_PAD_ALWAYS,
		gst_static_caps_get(&mpf_utterance_caps));

  klass->padtemplates = g_list_append(klass->padtemplates, template);
}

/**
 * mpf_utterance_add_output:
 * @name: named template pad.
 *
 * Adds a named template output pad to class of the current #MpfComponent.
 */
void mpf_utterance_add_output (const gchar *name) {
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;

  template = gst_pad_template_new(name, GST_PAD_SRC, GST_PAD_ALWAYS,
		gst_static_caps_get(&mpf_utterance_caps));

  klass->padtemplates = g_list_prepend(klass->padtemplates, template);
}

static GstBuffer * mpf_utterance_find_or_make_buffer(MpfComponent *component,
MpfUtterance *utterance) {
  GstBuffer *buf;
  gint size;

  // If the buffer already exists for this iplimage, return it
  if ((buf = mpf_component_find_buffer(component,utterance)))
    return buf;

  // Otherwise we need to make one
  buf = gst_buffer_new();
  size = sizeof(MpfUtterance);
  size += utterance->textlen;
  gst_buffer_set_data(buf,(guint8 *)utterance,size);
  return buf;
}

/**
 * mpf_utterance_push:
 * @padname: named template pad.
 * @utterance: a valid #MpfUtterance.
 *
 * Pushes a #GstBuffer to the named pad of the current #MpfComponent.
 */
void mpf_utterance_push (const gchar *padname, MpfUtterance *utterance) {
  MpfComponent *component = mpf_component_get_curcomponent();
  GstPad *pad;
  MpfComponentPadInfo *padinfo;
  GstBuffer *buf;

  pad = gst_element_get_pad(GST_ELEMENT(component),padname);
  padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);
  buf = mpf_utterance_find_or_make_buffer(component,utterance);

  padinfo->outbuf = buf; // gst_pad_push(pad,buf);
}

/**
 * mpf_utterance_new:
 * @id: utterance id.
 * @score: utterance score.
 * @text: utterance text.
 *
 * Creates a new #MpfUtterance and associates with the current #MpfComponent.
 *
 * Returns: an new #MpfUtterance.
 */
MpfUtterance *
mpf_utterance_new (guint32 id, gint32 score, gchar *text) {
  MpfComponent *component = mpf_component_get_curcomponent();
  int textlen, size;
  MpfUtterance *utterance;
  GstBuffer *newbuf;

  textlen = strlen(text);
  size = sizeof(MpfUtterance) + textlen + 1;
  newbuf = gst_buffer_new_and_alloc(size);
  utterance = (MpfUtterance *)GST_BUFFER_DATA(newbuf);
  utterance->text = ((char *)utterance + sizeof(MpfUtterance));

  utterance->id = id;
  utterance->score = score;
  utterance->textlen = textlen;
  strcpy(utterance->text, text);

  mpf_component_list_buffer(component,newbuf);

  return utterance;
}

/**
 * mpf_utterance_serialize:
 * @utterance: a valid #MpfUtterance.
 *
 *
 * Returns: an #MpfUtterance.
 */
MpfUtterance *
mpf_utterance_serialize(MpfUtterance *utterance) {
  return utterance;
}

/**
 * mpf_utterance_make_writable:
 * @utterance: a valid #MpfUtterance.
 *
 * Returns a writeable #GstBuffer associated with an #MpfUtterance or creates
 * a new writeable #GstBuffer and associates with the current #MpfComponent before
 * returning it.
 *
 * Returns: an #MpfUtterance.
 */
MpfUtterance *
mpf_utterance_make_writable(MpfUtterance *utterance) {
  MpfComponent *component = mpf_component_get_curcomponent();
  GstBuffer *buf, *newbuf;

  /* See if there's a buffer associated with this image */
  buf = mpf_component_find_buffer(component,(gpointer)utterance);

  /* If there is a buffer, we need to possibly make a copy of it */
  if (buf) {
    if (gst_buffer_is_writable(buf)) {
      return utterance;
    } else {
      newbuf = gst_buffer_make_writable(buf);
      mpf_component_list_buffer(component, newbuf);
      return (MpfUtterance *)GST_BUFFER_DATA(newbuf);
    }

  /* Otherwise we just have a bare image, we're by definition the only owner */
  } else {
    return utterance;
  }
}
