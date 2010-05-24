/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>
#include <gst/gst.h>
/**
 * SECTION: mpf-opencv
 * @short_description:
 */
#include <mpf/mpf-component.h>

#include "mpf-objlist.h"

GstStaticCaps mpf_objlist_caps = GST_STATIC_CAPS ( MPF_OBJLIST_CAPS );

/**
 * mpf_iplimage_add_input:
 * @name: name of the input pad to add
 *
 * Add an input pad to the component of the given name, with "match-all" caps.
 */
void
mpf_objlist_add_input (const gchar *name)
{
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;

  template = gst_pad_template_new(name, GST_PAD_SINK, GST_PAD_ALWAYS,
    gst_static_caps_get(&mpf_objlist_caps));

  klass->padtemplates = g_list_append(klass->padtemplates, template);
}

/**
 * mpf_iplimage_add_output:
 * @name: name of the output pad to add
 *
 * Add an input pad to the component of the given name, with "match-all" caps.
 */
void
mpf_objlist_add_output (const gchar *name)
{
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;

  template = gst_pad_template_new(name, GST_PAD_SRC, GST_PAD_ALWAYS,
    gst_static_caps_get(&mpf_objlist_caps));

  klass->padtemplates = g_list_prepend(klass->padtemplates, template);
}

/**
 * mpf_objlist_get_buffer:
 * @component: a #MpfComponent to find the objectlist in
 * @list: the objectlist to find the associated buffer for
 *
 * Returns a #GstBuffer that corresponds to the #MpfObjectList passed to the
 * function.  This buffer either already exists and is returned as is,
 * or is created on demand and registered with the component appropriately.
 *
 * Returns: the #GstBuffer associated with the objectlist
 */
GstBuffer *
mpf_objlist_get_buffer(MpfComponent *component, MpfObjectList *list)
{
  GstBuffer *buf;
  gint size;

  /* If the buffer already exists for this iplimage, return it */
  if ((buf = mpf_component_find_buffer(component,list)))
    return buf;

  /* Otherwise we need to make one */
  buf = gst_buffer_new();
  size = sizeof(MpfObjectList);
  size += sizeof(MpfObjectListEntry) * list->count;
  gst_buffer_set_data(buf,(guint8 *)list,size);
  return buf;
}

/**
 * mpf_objlist_new:
 * @numentries: a count of how many objects should be allocated
 *
 * Constructs a new objectlist with the given number of free slots.
 *
 * Returns: the newly allocated #MpfObjectList
 */
MpfObjectList *
mpf_objlist_new (gint numentries)
{
  MpfComponent *component = mpf_component_get_curcomponent();
  int listsize;
  MpfObjectList *list;
  GstBuffer *newbuf;

  listsize = sizeof(MpfObjectList) + sizeof(MpfObjectListEntry)*numentries;
  newbuf = gst_buffer_new_and_alloc(listsize);
  list = (MpfObjectList *)GST_BUFFER_DATA(newbuf);

  list->count = numentries;

  mpf_component_list_buffer(component, newbuf);

  return list;
}

/**
 * mpf_objlist_serialize:
 * @list: the #MpfObjectList to be serialized
 *
 * Returns the objectlist in a form that can be transmitted as a block of contiguous
 * bytes.  NOT YET IMPLEMETED!!!
 *
 * Returns: a serialized #MpfObjectList
 */
MpfObjectList *
mpf_objlist_serialize (MpfObjectList *list) {
  return list;
}

/**
 * mpf_objlist_make_writable:
 * @list: an #MpfObjectList to make writable
 *
 * Returns a pointer to a registered #MpfObjectList that has been made
 * writable for the component.  If the component is the sole owner of
 * the list at this point, the same list is returned.  If there are
 * other users of the list, a copy will be made that is owned solely by
 * the current component.
 *
 * Returns: an #MpfObjectList that may be the original or a copy, but is writable
 */
MpfObjectList *
mpf_objlist_make_writable(MpfObjectList *list)
{
  MpfComponent *component = mpf_component_get_curcomponent();
  GstBuffer *buf, *newbuf;

  /* See if there's a buffer associated with this image */
  buf = mpf_component_find_buffer(component,(gpointer)list);

  /* If there is a buffer, we need to possibly make a copy of it */
  if (buf) {
    if (gst_buffer_is_writable(buf)) {
      return list;
    } else {
      newbuf = gst_buffer_make_writable(buf);
      mpf_component_list_buffer(component, newbuf);
      return (MpfObjectList *)GST_BUFFER_DATA(newbuf);
    }
  /* Otherwise we just have a bare image, we're by definition the only owner */
  }
  else
  {
    return list;
  }
}
