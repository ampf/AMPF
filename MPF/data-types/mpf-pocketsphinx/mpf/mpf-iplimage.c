/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>
#include <gst/gst.h>
#include <gst/video/video.h>

#include <mpf/mpf-component.h>

#include "mpf-iplimage.h"

GstStaticCaps mpf_iplimage_caps = GST_STATIC_CAPS ( MPF_IPLIMAGE_CAPS );


void
mpf_iplimage_add_input (const gchar *name, MpfIplImageFormatList formatlist)
{
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;
  GstCaps *caps;

  caps = mpf_iplimage_formatlist_to_caps(formatlist);
  template = gst_pad_template_new(name, GST_PAD_SINK, GST_PAD_ALWAYS, caps);

  mpf_component_add_input(klass,template);
}

void
mpf_iplimage_add_output (const gchar *name, MpfIplImageFormatList formatlist)
{
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;
  GstCaps *caps;

  caps = mpf_iplimage_formatlist_to_caps(formatlist);
  template = gst_pad_template_new(name, GST_PAD_SRC, GST_PAD_ALWAYS, caps);

  mpf_component_add_output(klass,template);
}

GstBuffer *
mpf_iplimage_get_buffer(MpfComponent *component, IplImage *iplimage)
{
  GstBuffer *buf;

  // If *comp is NULL, go pick it out of the thread state
  if (component == NULL) {
    component = mpf_component_get_curcomponent();
  }

  // If the buffer already exists for this iplimage, return it
  if ((buf = mpf_component_find_buffer(component,iplimage)))
    return buf;

  // Otherwise we need to make one
  buf = gst_buffer_new();
  gst_buffer_set_data(buf,(guint8 *)iplimage,sizeof(IplImage)+iplimage->imageSize);

  // Add it to the component's private list
  mpf_component_list_buffer(component, buf);

  return buf;
}


void
mpf_iplimage_rectify_imagedata(MpfComponent *comp, IplImage *iplimage)
{
  iplimage->imageData = ((char *)iplimage + sizeof(IplImage));
}


IplImage *
mpf_iplimage_make_writable_func(IplImage *iplimage)
{
  MpfComponent *component = mpf_component_get_curcomponent();
  GstBuffer *buf, *newbuf;

  // See if there's a buffer associated with this image
  buf = mpf_component_find_buffer(component,(gpointer)iplimage);

  // If there is a buffer, we need to possibly make a copy of it
  if (buf) {
    if (gst_buffer_is_writable(buf)) {
      return iplimage;
    } else {
      newbuf = gst_buffer_make_writable(buf);
      mpf_iplimage_rectify_imagedata(component, (IplImage*)GST_BUFFER_DATA(newbuf));
      mpf_component_list_buffer(component, newbuf);
      return (IplImage *)GST_BUFFER_DATA(newbuf);
    }

  // Otherwise we just have a bare image, we're by definition the only owner
  } else {
    return iplimage;
  }
}

IplImage *
mpf_iplimage_make_from_img(IplImage *refimg)
{
  MpfComponent *component = mpf_component_get_curcomponent();
  GstBuffer *refbuf, *newbuf;
  IplImage *newimg;
  GstStructure *capstruct;
  int width, height, depth, channels;

  refbuf = mpf_component_find_buffer(component,refimg);
  newbuf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(refbuf));

  GST_BUFFER_TIMESTAMP (newbuf) = GST_BUFFER_TIMESTAMP (refbuf);
//  fprintf(stderr,"setting outbound buffer caps to %s\n",gst_caps_to_string(GST_BUFFER_CAPS (refbuf)));
  gst_buffer_set_caps(newbuf, GST_BUFFER_CAPS (refbuf));

  capstruct = gst_caps_get_structure(GST_BUFFER_CAPS (refbuf), 0);
  gst_structure_get_int (capstruct, "width", &width);
  gst_structure_get_int (capstruct, "height", &height);
  gst_structure_get_int (capstruct, "channels", &channels);
  depth = 8;

//  fprintf(stderr,"make_from() creating image of %dx%d, depth %d, %d channels\n",width,height,depth,channels);
  newimg = (IplImage *)GST_BUFFER_DATA(newbuf);
  cvInitImageHeader(newimg, cvSize(width,height),IPL_DEPTH_8U,channels,0,4);
  newimg->widthStep = width * depth / 8 * channels;
  newimg->imageData = ((char *)newimg + sizeof(IplImage));

  mpf_component_list_buffer(component, newbuf);

  return newimg;
}

IplImage *
mpf_iplimage_make_from_pad(const gchar *padname)
{
  MpfComponent *component = mpf_component_get_curcomponent();
  GstPad *pad;
  GstCaps *caps;
  GstBuffer *refbuf, *newbuf;
  IplImage *newimg;
  GstStructure *capstruct;
  int width, height, depth, channels;

  pad = gst_element_get_pad(GST_ELEMENT(component),padname);
  caps = gst_pad_get_allowed_caps(pad);
  mpf_iplimage_caps_to_formatlist(caps);

  return NULL;

  newbuf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(refbuf));

  GST_BUFFER_TIMESTAMP (newbuf) = GST_BUFFER_TIMESTAMP (refbuf);
  fprintf(stderr,"setting outbound buffer caps to %s\n",gst_caps_to_string(GST_BUFFER_CAPS (refbuf)));
  gst_buffer_set_caps(newbuf, GST_BUFFER_CAPS (refbuf));

  capstruct = gst_caps_get_structure(GST_BUFFER_CAPS (refbuf), 0);
  gst_structure_get_int (capstruct, "width", &width);
  gst_structure_get_int (capstruct, "height", &height);
  gst_structure_get_int (capstruct, "channels", &channels);
  depth = 8;

  fprintf(stderr,"make_from() creating image of %dx%d, depth %d, %d channels\n",width,height,depth,channels);
  newimg = (IplImage *)GST_BUFFER_DATA(newbuf);
  cvInitImageHeader(newimg, cvSize(width,height),IPL_DEPTH_8U,channels,0,4);
  newimg->widthStep = width * depth / 8 * channels;
  newimg->imageData = ((char *)newimg + sizeof(IplImage));

  mpf_component_list_buffer(component, newbuf);

  return newimg;
}


uint64_t
mpf_iplimage_formatlist_real(uint32_t type1, ...)
{
  uint64_t formatlist = MPF_IPLIMAGE_FORMAT_ID;
  va_list args;
  uint32_t prev = -1;
  uint64_t arg64;

  va_start (args, type1);

  while (type1 != MPF_IPLIMAGE_FORMAT_END32) {

    if (type1 & MPF_IPLIMAGE_FORMAT_ID32) {
      arg64 = prev | ((uint64_t)type1 << 32);

      formatlist |= arg64;

      prev = -1;
    } else {
      if (prev != -1) {
        formatlist |= ((uint64_t)1 << prev);
      }

      prev = type1;
    }

    type1 = va_arg (args, uint32_t);
  }
  va_end (args);

  if (prev != -1) {
    formatlist |= ((uint64_t)1 << prev);
  }

  return formatlist;
}


static gchar *cvformatstrings[] = {
  "8U", "8S", "16U", "16S", "32S", "32F", "64F"
};

GstCaps *
mpf_iplimage_formatlist_to_caps(MpfIplImageFormatList formatlist)
{
  GstCaps *caps, *partialcaps;
  gchar *capstring;
  int c,f;

  if (formatlist == MPF_IPLIMAGE_FORMAT_ANY) {
//    fprintf(stderr,"mpf_iplimage_formatlist_to_caps(): any IplImage\n");
    caps = gst_static_caps_get(&mpf_iplimage_caps);
  } else {

    caps = gst_caps_new_empty();

    // Loop through all 4 channel-count options
    for (c=0;c<4;c++) {
      // Go through each format and add it
      for (f=0;f<7;f++) {
        if (formatlist & (1<<(c*8+f))) {
//        fprintf(stderr,"mpf_iplimage_formatlist_to_caps(): adding fmt %d, chans %d\n",f,c);
          capstring = g_strdup_printf("video/x-raw-iplimage, "
				"format = (string) %s, "
				"channels = (int) %d, "
				"width = " GST_VIDEO_SIZE_RANGE ", "
				"height = " GST_VIDEO_SIZE_RANGE,
				cvformatstrings[f], c+1);
          partialcaps = gst_caps_from_string (capstring);
          g_free(capstring);
          gst_caps_append(caps, partialcaps);
        }
      }
    }
  }

//  fprintf(stderr,"%s\n",gst_caps_to_string(caps));

  return caps;
}

/* FIXME incomplete */
MpfIplImageFormatList 
mpf_iplimage_caps_to_formatlist(GstCaps *caps)
{
  MpfIplImageFormatList formats = MPF_IPLIMAGE_FORMAT_ANY;
  int i;
  GstStructure *capstruct;
  const gchar *format;
  gint channels;

  fprintf(stderr,"%s\n",gst_caps_to_string(caps));

  for (i=0;i<gst_caps_get_size(caps);i++) {
    capstruct = gst_caps_get_structure(caps, i);
    format = gst_structure_get_string (capstruct, "format");
    gst_structure_get_int (capstruct, "channels", &channels);
    fprintf(stderr,"caps_to_formatlist %d: '%s' x%d\n",i,format,channels);
  }
  return formats;
}
