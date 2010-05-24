/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-component.h>


/**
 * mpf_voidstar_add_input:
 * @name: name of the pad to add to the component
 *
 * Add an output pad to the component of the given name, with wildcard caps.
 */
#define mpf_voidstar_add_input(name) \
  mpf_component_add_input_with_caps(NULL,name,GST_CAPS_ANY);

/**
 * mpf_voidstar_add_input_sometimes:
 * @name: name of the pad to add to the component
 *
 * Add an output pad to the component of the given name, with wildcard caps.
 */
#define mpf_voidstar_add_input_sometimes(name) \
  mpf_component_add_input_with_caps_presence(NULL,name,GST_CAPS_ANY,GST_PAD_SOMETIMES);

/**
 * mpf_voidstar_add_input_request:
 * @name: name of the pad to add to the component
 *
 * Add an output pad to the component of the given name, with wildcard caps.
 */
#define mpf_voidstar_add_input_request(name) \
  mpf_component_add_input_with_caps_presence(NULL,name,GST_CAPS_ANY,GST_PAD_REQUEST);

/**
 * mpf_voidstar_add_output:
 * @name: name of the pad to add to the component
 *
 * Add an output pad to the component of the given name, with wildcard caps.
 */
#define mpf_voidstar_add_output(name) \
  mpf_component_add_output_with_caps(NULL,name,GST_CAPS_ANY);

/**
 * mpf_voidstar_add_input_with_caps:
 * @name: name of the input pad to add
 * @caps: the GStreamer-style caps string for the pad
 *
 * Add an input pad to the component of the given name, with the given caps.
 */
#define mpf_voidstar_add_input_with_caps(name,caps) \
  mpf_component_add_input_with_capstr(NULL,name,caps)

/**
 * mpf_voidstar_add_output_with_caps:
 * @name: name of the output pad to add
 * @caps: the GStreamer-style caps string for the pad
 *
 * Add an output pad to the component of the given name, with the given caps.
 */
#define mpf_voidstar_add_output_with_caps(name,caps) \
  mpf_component_add_output_with_capstr(NULL,name,caps)

GstBuffer *mpf_voidstar_get_buffer(MpfComponent *component, void *data);

/**
 * mpf_voidstar_alloc:
 * @size size of data to allocate
 * Returns: the void* data allocated, which is associated with a GstBuffer
 */
void* mpf_voidstar_alloc(size_t size);

/**
 * mpf_voidstar_stralloc:
 * @string string data to allocate and copy into a voidstar buffer.
 * Returns: the void* data allocated, which is associated with a GstBuffer
 */
void* mpf_voidstar_stralloc(const gchar* string);

/**
 * mpf_voidstar_pull:
 * @padname: the name of an input pad to pull from
 *
 * Pull a pointer from the input pad, for processing in the component.  A
 * reference is transferred to the component code.
 *
 * Returns: an pointer to the data pulled
 */
#define mpf_voidstar_pull(padname) (void *)mpf_component_pull(NULL,padname)

gint mpf_voidstar_get_length(void *ptr);
void mpf_voidstar_set_length(void *ptr, gint len);

#define mpf_voidstar_is_linked(padname) mpf_component_is_linked(NULL, padname)

/**
 * mpf_voidstar_push:
 * @padname: the name of an output pad to push the data out to
 * @data: the pointer of data to push
 *
 * Send a pointer out the given output pad, to the next element in the pipeline.
 * This takes ownership of a reference.
 */
#define mpf_voidstar_push(padname,data) \
  mpf_component_push_buffer(NULL,padname,mpf_voidstar_get_buffer(NULL,data))

#define mpf_voidstar_send_outbuffers() \
  mpf_component_send_outbuffers(mpf_component_get_curcomponent());


/**
 * mpf_voidstar_push_pad:
 * @padname: the name of an output pad to push the data out to
 * @data: the pointer of data to push
 *
 * Send a pointer out the given output pad, to the next element in the pipeline.
 * This takes ownership of a reference.  The buffer is allocated by the pad,
 * which therefore allows multiple pushes to occur in a single cycle.
 */
#define mpf_voidstar_push_pad(padname,data,capspad) \
  mpf_component_push_buffer_pad(NULL,padname,mpf_voidstar_get_buffer(NULL,data),capspad)


void mpf_voidstar_push_with_len(gchar *padname, void *data, gint len);

/**
 * mpf_voidstar_ref:
 * @data: the pointer to increment the reference count of
 *
 * Increment the reference count of the pointer, used to take ownership.
 */
#define mpf_voidstar_ref(data) mpf_component_data_ref((gpointer)data)

/**
 * mpf_voidstar_unref:
 * @data: the pointer to decrement the reference count of
 *
 * Decrement the reference count of the pointer.  If the reference count drops to
 * zero, the data will be freed.  After an _unref call, you generally may not
 * assume the pointer still points to valid memory.
 */
#define mpf_voidstar_unref(data) mpf_component_data_unref((gpointer)data)
