/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdint.h>
/**
 * SECTION: mpf-newdatatype
 * @short_description:
 */
#include <mpf/mpf-component.h>
/* include your specific data-type headers here */
//#include <newdatatype.h>
/* remove this and replace as necessary with the real data type */
#define Newdatatype char

/* define specific data-type caps here */
#define MPF_NEWDATATYPE_CAPS						\
	"video/x-raw-newdatatype, "					\
	"channels = (int) [ 1, 4 ], "					\
	"width = " GST_VIDEO_SIZE_RANGE ", "				\
	"height = " GST_VIDEO_SIZE_RANGE

GstStaticCaps mpf_newdatatype_caps;


/**
 * mpf_newdatatype_pull:
 * @padname: the name of an input pad to pull from
 *
 * Pulls a #Newdatatype from the input pad, for processing in the component.  A
 * reference is transferred to the component code.
 *
 * Returns: a #Newdatatype
 */
#define mpf_newdatatype_pull(padname) \
	((Newdatatype *)mpf_component_pull(NULL, padname))
/**
 * mpf_newdatatype_push:
 * @padname: the name of an output pad to push the #Newdatatype out to
 * @image: the #Newdatatype to push
 *
 * Sends a #Newdatatype out the given output pad to the next element in the pipeline.
 * This takes ownership of a reference.
 */
#define mpf_newdatatype_push(padname,image) \
	mpf_component_push_buffer(NULL, padname, mpf_newdatatype_get_buffer(NULL,image))

/**
 * mpf_newdatatype_ref:
 * @image: the #Newdatatype to increment the reference count of
 *
 * Increments the reference count of the image, used to take ownership.
 */
#define mpf_newdatatype_ref(image) mpf_component_data_ref(image)

/**
 * mpf_newdatatype_unref:
 * @image: the #Newdatatype to decrement the reference count of
 *
 * Decrement the reference count of the image.  If the reference count drops to
 * zero, the image will be freed.  After an _unref call, you generally may not
 * assume the image pointer still points to valid memory.
 */
#define mpf_newdatatype_unref(image) mpf_component_data_unref(image)

/**
 * mpf_newdatatype_make_writable:
 * @image: the #Newdatatype to make writable
 *
 * Makes certain that a #Newdatatype can be written to by the current context.  If the
 * reference count is one, it is assumed to belong solely to the current context,
 * and is returned as is.  Otherwise, a complete copy of the image is made with the
 * reference count now owned by the current context.
 *
 * Returns: a #Newdatatype that is writable (and may or may not be the same as the argument)
 */
#define mpf_newdatatype_make_writable(image)				\
(Newdatatype *)mpf_component_make_data_writable((gpointer)image,	\
		(MpfComponentRectifyData)mpf_newdatatype_rectify_imagedata)

/* --- prototypes --- */
GstBuffer * mpf_newdatatype_get_buffer(MpfComponent *component, Newdatatype *newdatatype);

void mpf_newdatatype_add_input(const gchar *name);
void mpf_newdatatype_add_output(const gchar *name);

void mpf_newdatatype_rectify_imagedata(MpfComponent *comp, Newdatatype *image);

Newdatatype * mpf_newdatatype_make_from_img(Newdatatype *refimg);
Newdatatype * mpf_newdatatype_make_from_pad(const gchar *padname);

