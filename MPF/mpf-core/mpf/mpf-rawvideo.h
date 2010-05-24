/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-component.h>

typedef enum {
  MPF_RAWVIDEO_RGB24,
  MPF_RAWVIDEO_BGR24,
  MPF_RAWVIDEO_RGB32,
  MPF_RAWVIDEO_BGR32,
  MPF_RAWVIDEO_I420,
} MpfVideoFormat;

/**
 * mpf_rawvideo_add_input:
 * @name: name of the pad to add to the component
 *
 * Add an output pad to the component of the given name, 
 */
#define mpf_rawvideo_add_input(name) \
        mpf_component_add_input_with_caps(NULL,name,GST_CAPS_ANY);

/**
 * mpf_voidstar_add_output:
 * @name: name of the pad to add to the component
 *
 * Add an output pad to the component of the given name, with wildcard caps.
 */
#define mpf_rawvideo_add_output(name) \
        mpf_component_add_output_with_caps(NULL,name,GST_CAPS_ANY);

void mpf_rawvideo_add_input_with_format (const gchar *padname, MpfVideoFormat format,gint minwidth,gint maxwidth,gint minheight,gint maxheight);
void mpf_rawvideo_add_output_with_format (const gchar *padname, MpfVideoFormat format,gint minwidth,gint maxwidth,gint minheight,gint maxheight);

#define mpf_rawvideo_add_caps(padname,caps) \
	mpf_component_add_caps(NULL,padname,caps);
#define mpf_rawvideo_remove_caps(padname,caps) \
	mpf_component_remove_caps(NULL,padname,caps);

GstBuffer *mpf_rawvideo_get_buffer(MpfComponent *component, void *data);


/**
 * mpf_rawvideo_pull:
 * @padname: the name of an input pad to pull from
 *
 * Pull a pointer from the input pad, for processing in the component.  A
 * reference is transferred to the component code.
 *
 * Returns: a void pointer to the image data
 */
#define mpf_rawvideo_pull(padname) (void *)mpf_component_pull(NULL,padname)

/**
 * mpf_rawvideo_push:
 * @padname: the name of an output pad to push the image out to
 * @data: the pointer of image to push
 *
 * Send an image out the given output pad, to the next element in the pipeline.
 * This takes ownership of a reference.
 */
#define mpf_rawvideo_push(padname,data) \
	mpf_component_push_buffer(NULL,padname,mpf_rawvideo_get_buffer(NULL,data))

/**
 * mpf_rawvideo_ref:
 * @data: the image to increment the reference count of
 *
 * Increment the reference count of the pointer, used to take ownership.
 */
#define mpf_rawvideo_ref(data) mpf_component_data_ref((gpointer)data)

/**
 * mpf_rawvideo_unref:
 * @image: the image to decrement the reference count of
 *
 * Decrement the reference count of the image.  If the reference count drops to
 * zero, the data will be freed.  After an _unref call, you generally may not
 * assume the pointer still points to valid memory.
 */
#define mpf_rawvideo_unref(data) mpf_component_data_unref((gpointer)data)
