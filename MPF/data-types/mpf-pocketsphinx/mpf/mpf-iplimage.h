/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdint.h>

#include <mpf/mpf-component.h>
#include <cv.h>

typedef guint64 MpfIplImageFormatList;


#define MPF_IPLIMAGE_CAPS						\
	"video/x-raw-iplimage, "					\
	"format = (string) { 8U, 8S, 16U, 16S, 32S, 32F, 64F }, "	\
	"channels = (int) [ 1, 4 ], "					\
	"width = " GST_VIDEO_SIZE_RANGE ", "				\
	"height = " GST_VIDEO_SIZE_RANGE

GstStaticCaps mpf_iplimage_caps;

void mpf_iplimage_add_input (const gchar *name, MpfIplImageFormatList formatlist);
void mpf_iplimage_add_output (const gchar *name, MpfIplImageFormatList formatlist);

#define MPF_IPLIMAGE_FORMAT_ID		(uint64_t)0x8000000000000000LL
#define MPF_IPLIMAGE_FORMAT_ID32	(uint32_t)0x80000000
#define MPF_IPLIMAGE_FORMAT_END		(uint64_t)0xffffffffffffffffLL
#define MPF_IPLIMAGE_FORMAT_END32	(uint32_t)0xffffffff
#define MPF_IPLIMAGE_FORMAT_ANY		(uint64_t)0LL

MpfIplImageFormatList mpf_iplimage_formatlist_real(uint32_t type1, ...);
#define mpf_iplimage_formatlist(...) mpf_iplimage_formatlist_real( __VA_ARGS__ ,MPF_IPLIMAGE_FORMAT_END32)

MpfIplImageFormatList mpf_iplimage_formatlist_merge_real(MpfIplImageFormatList list1, ...);
#define mpf_iplimage_formatlist_merge(lists...) mpf_iplimage_formatlist_real( ##lists , 0)

GstBuffer *
mpf_iplimage_get_buffer(MpfComponent *component, IplImage *iplimage);

#define mpf_iplimage_pull(padname) \
	((IplImage *)mpf_component_pull(NULL, padname))
#define mpf_iplimage_push(padname,iplimage) \
	mpf_component_push_buffer(NULL, padname, mpf_iplimage_get_buffer(NULL,iplimage))

//IplImage *mpf_iplimage_make_writable(IplImage *img);
IplImage *mpf_iplimage_make_from_img(IplImage *refimg);
IplImage *mpf_iplimage_make_from_pad(const gchar *padname);

GstCaps *mpf_iplimage_formatlist_to_caps(MpfIplImageFormatList formatlist);
MpfIplImageFormatList mpf_iplimage_caps_to_formatlist(GstCaps *caps);

#define mpf_iplimage_ref(iplimage) mpf_component_data_ref(iplimage)
#define mpf_iplimage_unref(iplimage) mpf_component_data_unref(iplimage)

void mpf_iplimage_rectify_imagedata(MpfComponent *comp, IplImage *image);

#define mpf_iplimage_make_writable(iplimage)				\
(IplImage *)mpf_component_make_data_writable((gpointer)iplimage,	\
				(MpfComponentRectifyData)mpf_iplimage_rectify_imagedata)
