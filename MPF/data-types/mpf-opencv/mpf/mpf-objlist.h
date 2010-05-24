/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdint.h>

/**
 * SECTION: mpf-opencv
 * @short_description:
 */
#include <mpf/mpf-component.h>

#define MPF_OBJLIST_TYPE_FACE 1

/* typedefs & structures */
typedef struct _MpfObjectList MpfObjectList;
typedef struct _MpfObjectListEntry MpfObjectListEntry;

/**
 * MpfObjectList:
 *
 * All the fields in the <structname>MpfObjectList</structname> structure are private
 * to the #MpfObjectList implementation and should never be accessed directly.
 */
struct _MpfObjectListEntry {
  /*< private >*/
  gint type;
  guint x,y,w,h;
  guint quality;
  guint64 uid;
  gchar *name;
};

/**
 * MpfObjectListEntry:
 *
 * All the fields in the <structname>MpfObjectListEntry</structname> structure are private
 * to the #MpfObjectListEntry implementation and should never be accessed directly.
 *
 */
struct _MpfObjectList {
  /*< private >*/
  gint type;
  gint count;
  MpfObjectListEntry list[0];
};

#define MPF_OBJLIST_CAPS \
  "application/x-mpf-objectlist"

GstStaticCaps mpf_objlist_caps;

/* --- defines --- */
/**
 * mpf_objlist_pull:
 * @padname: the name of an input pad to pull from
 *
 * Pull an #MpfObjectList from the input pad, for processing in the component.  A
 * reference is transferred to the component code.
 *
 * Returns: an #IplImage
 */
#define mpf_objlist_pull(padname) \
  ((MpfObjectList *)mpf_component_pull(NULL,padname))
/**
 * mpf_objlist_push:
 * @padname: the name of an output pad to push the #MpfObjectList out to
 *
 * Send an #MpfObjectList out the given output pad, to the next element in the
 * pipeline. This takes ownership of a reference.
 */
#define mpf_objlist_push(padname,list) \
  mpf_component_push_buffer(NULL, padname, mpf_objlist_get_buffer(NULL,list))
/**
 * mpf_objlist_ref:
 * @image: the #MpfObjectList to increment the reference count of
 *
 * Increment the reference count of the list, used to take ownership.
 */
#define mpf_objlist_ref(objlist) mpf_component_data_ref(objlist);
/**
 * mpf_objlist_unref:
 * @objlist: the #MpfObjectList to decrement the reference count of
 *
 * Decrement the reference count of the list.  If the reference count drops to
 * zero, the list will be freed.  After an _unref call, you generally may not
 * assume the list pointer still points to valid memory.
 */
#define mpf_objlist_unref(objlist) mpf_component_data_unref(objlist);

/* --- prototypes --- */
void mpf_objlist_add_input (const gchar *name);
void mpf_objlist_add_output (const gchar *name);

MpfObjectList *mpf_objlist_new (gint numentries);
MpfObjectList *mpf_objlist_serialize (MpfObjectList *list);

MpfObjectList *mpf_objlist_make_writable(MpfObjectList *list);

GstBuffer *mpf_objlist_get_buffer(MpfComponent *component, MpfObjectList *list);

