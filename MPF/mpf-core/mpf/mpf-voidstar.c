/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <string.h>

/**
 * SECTION: mpf-voidstar
 * @short_description:
 */
#include "mpf-voidstar.h"


GstStaticCaps mpf_voidstar_caps = GST_STATIC_CAPS_ANY;


/*
 * mpf_voidstar_get_buffer:
 * @component: a #MpfComponent to find the image in (or NULL for current)
 * @data: the pointer to find the associated buffer for
 *
 * Returns a #GstBuffer that corresponds to the pointer passed to the
 * function.  This buffer either already exists and is returned as is,
 * or is created on demand and registered with the component appropriately.
 *
 * Returns: the #GstBuffer associated with the pointer
 */
GstBuffer *
mpf_voidstar_get_buffer(MpfComponent *component, void *data)
{
  GstBuffer *buf;

  if (component == NULL)
    component = mpf_component_get_curcomponent();

  // If the buffer already exists for this data, return it
  if ((buf = mpf_component_find_buffer(component,data)))
    return buf;

  // Otherwise we need to make a new one
  buf = gst_buffer_new();
  gst_buffer_set_data(buf,(guint8 *)data,4);

  // Add it to the component's private list
  mpf_component_list_buffer(component, buf);

  return buf;
}

/**
 * mpf_voidstar_get_length:
 * @data: the pointer to find the length of
 *
 * Retrieve the length of a pointer associated with the given pad.
 *
 * Returns: the registered length of the pointer, or -1 if invalid
 */
gint
mpf_voidstar_get_length(void *data) {
  MpfComponent *component;
  GstBuffer *buf;

  component = mpf_component_get_curcomponent();

  // Get the registered buffer, otherwise return -1 if it isn't found
  if (! (buf = mpf_component_find_buffer(component,data)))
    return -1;

  // Return the length of the GstBuffer
  return GST_BUFFER_SIZE(buf);
}

/**
 * mpf_voidstar_set_length:
 * @data: the pointer to set the length to
 * @len: the new length of the pointer's data
 *
 * Set the length of the data referenced by the pointer.
 */
void
mpf_voidstar_set_length(void *data, gint len)
{
  MpfComponent *component;
  GstBuffer *buf;

  component = mpf_component_get_curcomponent();

  // Get the registered buffer, otherwise return if it isn't found
  if (! (buf = mpf_component_find_buffer(component,data)))
    return;

  GST_BUFFER_SIZE(buf) = len;
}

/**
 * mpf_voidstar_push_with_len:
 * @padname: the name of an output pad to push the data out to
 * @data: the pointer of data to push
 * @len: the length of the data
 *
 * Send a pointer with data of a given length out the given output pad, to the next
 * element in the pipeline.  This takes ownership of a reference.
 */
void
mpf_voidstar_push_with_len(gchar *padname, void *data, gint len)
{
  MpfComponent *component;
  GstBuffer *buf;

  component = mpf_component_get_curcomponent();

  // Get the buffer
  buf = mpf_voidstar_get_buffer(component,data);
  if (buf == NULL)
    return;

  // Set its length
  GST_BUFFER_SIZE(buf) = len;

  // Push it
  mpf_component_push_buffer(component,padname,buf);
}


GstCaps *
mpf_voidstar_get_caps(void *data)
{
  MpfComponent *component;
  GstBuffer *buf;

  component = mpf_component_get_curcomponent();

  // Get the buffer
  buf = mpf_voidstar_get_buffer(component,data);
  g_assert(buf != NULL);
  g_assert(GST_IS_BUFFER(buf));

  return GST_BUFFER_CAPS(buf);
}

void*
mpf_voidstar_alloc(size_t size) {
    void* buf = g_malloc(size);
    // Will always return a new buffer, associated with the new buf.
    mpf_voidstar_get_buffer(NULL, buf);
    mpf_voidstar_set_length(buf, size);
    return buf;
}

void*
mpf_voidstar_stralloc(const gchar *string) {
    size_t len = strlen(string);
    void* buf = g_malloc(len+1);
    memcpy(buf, string, len+1);
    // Will always return a new buffer, associated with the new buf.
    mpf_voidstar_get_buffer(NULL, buf);
    mpf_voidstar_set_length(buf, len+1);
    return buf;
}
