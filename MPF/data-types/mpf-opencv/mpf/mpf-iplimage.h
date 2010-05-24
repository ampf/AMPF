/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdint.h>

/**
 * SECTION: mpf-opencv
 * @short_description:
 */
#include <mpf/mpf-component.h>
#include <cv.h>

typedef guint64 MpfIplImageFormatList;


#define MPF_IPLIMAGE_CAPS						              \
	"video/x-raw-iplimage, "					              \
	"format = (string) { 8U, 8S, 16U, 16S, 32S, 32F, 64F }, " \
	"channels = (int) [ 1, 4 ], "					          \
	"width = " GST_VIDEO_SIZE_RANGE ", "				      \
	"height = " GST_VIDEO_SIZE_RANGE

GstStaticCaps mpf_iplimage_caps;

#define MPF_IPLIMAGE_FORMAT_ID		(uint64_t)0x8000000000000000LL
#define MPF_IPLIMAGE_FORMAT_ID32	(uint32_t)0x80000000
#define MPF_IPLIMAGE_FORMAT_END		(uint64_t)0xffffffffffffffffLL
#define MPF_IPLIMAGE_FORMAT_END32	(uint32_t)0xffffffff
#define MPF_IPLIMAGE_FORMAT_ANY		(uint64_t)0LL

/**
 * mpf_iplimage_formatlist:
 *
 */
#define mpf_iplimage_formatlist(...) \
	mpf_iplimage_formatlist_real( __VA_ARGS__ ,MPF_IPLIMAGE_FORMAT_END32)

/*
MpfIplImageFormatList mpf_iplimage_formatlist_merge_real(MpfIplImageFormatList list1, ...);
#define mpf_iplimage_formatlist_merge(lists...) mpf_iplimage_formatlist_real( ##lists , 0)
*/

/**
 * mpf_iplimage_pull:
 * @padname: the name of an input pad to pull from.
 *
 * Pulls an #IplImage from the input pad, for processing in the component.  A
 * reference is transferred to the component code.
 *
 * Returns: a valid #IplImage.
 */
#define mpf_iplimage_pull(padname) \
	((IplImage *)mpf_component_pull(NULL, padname))
/**
 * mpf_iplimage_push:
 * @padname: the name of an output pad to push the #IplImage out to
 * @image: the #IplImage to push
 *
 * Sends an #IplImage out the given output pad, to the next element in the pipeline.
 * This takes ownership of a reference.
 */
#define mpf_iplimage_push(padname,image) \
	mpf_component_push_buffer(NULL, padname, mpf_iplimage_get_buffer(NULL,image))
/**
 * mpf_iplimage_ref:
 * @image: the #IplImage to increment the reference count of
 *
 * Increments the reference count of the image, used to take ownership.
 */
#define mpf_iplimage_ref(image) mpf_component_data_ref(image)
/**
 * mpf_iplimage_unref:
 * @image: the #IplImage to decrement the reference count of
 *
 * Decrements the reference count of the image.  If the reference count drops to
 * zero, the image will be freed.  After an _unref call, you generally may not
 * assume the image pointer still points to valid memory.
 */
#define mpf_iplimage_unref(image) mpf_component_data_unref(image)
/**
 * mpf_iplimage_make_writable:
 * @image: the #IplImage to make writable
 *
 * Make certain that an #IplImage can be written to by the current context.  If the
 * reference count is one, it is assumed to belong solely to the current context,
 * and is returned as is.  Otherwise, a complete copy of the image is made with the
 * reference count now owned by the current context.
 *
 * Returns: an #IplImage that is writable (and may or may not be the same as the argument).
 */
#define mpf_iplimage_make_writable(image)				            \
(IplImage *)mpf_component_make_data_writable((gpointer)image,		\
				(MpfComponentRectifyData)mpf_iplimage_rectify_imagedata)

/* --- prototypes --- */
GstBuffer * mpf_iplimage_get_buffer(MpfComponent *component, IplImage *iplimage);

void mpf_iplimage_rectify_imagedata(MpfComponent *comp, IplImage *image);

void mpf_iplimage_add_input (const gchar *name, MpfIplImageFormatList formatlist);
void mpf_iplimage_add_output (const gchar *name, MpfIplImageFormatList formatlist);

IplImage * mpf_iplimage_make_from_img(IplImage *refimg);
IplImage * mpf_iplimage_make_from_pad(const gchar *padname);

GstCaps * mpf_iplimage_formatlist_to_caps(MpfIplImageFormatList formatlist);

MpfIplImageFormatList mpf_iplimage_caps_to_formatlist(GstCaps *caps);
MpfIplImageFormatList mpf_iplimage_formatlist_real(uint32_t type1, ...);

