/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>
#include <gst/gst.h>
#include <gst/video/video.h>
/**
 * SECTION: mpf-opencv
 * @short_description:
 */
#include <mpf/mpf-component.h>

#include "mpf-iplimage.h"

GstStaticCaps mpf_iplimage_caps = GST_STATIC_CAPS ( MPF_IPLIMAGE_CAPS );

/**
 * mpf_iplimage_add_input:
 * @name: name of the input pad to add.
 * @formatlist: a #MpfIplImageFormatList of the possible image types for
 * this input, should be a superset of all possible formats.
 *
 * Adds an input pad to the component of the given name, with a superset
 * list of all viable formats.  This format list may have to be shrunk
 * once parameters are set.
 */
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

/**
 * mpf_iplimage_add_output:
 * @name: name of the output pad to add
 * @formatlist: a #MpfIplImageFormatList of the possible image types for
 * this output, should be a superset of all possible formats
 *
 * Adds an output pad to the component of the given name, with a superset
 * list of all viable formats.  Once the parameters have been set and
 * the data starts to flow, the component must clarify exactly which
 * format is used with the mpf_iplimage_??? function.
 */
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

/*
 * mpf_iplimage_get_buffer:
 * @component: a #MpfComponent to find the image in (or %NULL for current)
 * @iplimage: the image to find the associated buffer for
 *
 * Returns a #GstBuffer that corresponds to the #IplImage passed to the
 * function.  This buffer either already exists and is returned as is,
 * or is created on demand and registered with the component appropriately.
 *
 * Returns: the #GstBuffer associated with the image.
 */
GstBuffer *
mpf_iplimage_get_buffer(MpfComponent *component, IplImage *iplimage)
{
  GstBuffer *buf;

  /* If @component is NULL, go pick it out of the thread state */
  if (component == NULL) {
    component = mpf_component_get_curcomponent();
  }

  /* If the buffer already exists for this iplimage, return it */
  if ((buf = mpf_component_find_buffer(component,iplimage)))
    return buf;

  /* Otherwise we need to make one */
  buf = gst_buffer_new();
  gst_buffer_set_data(buf,(guint8 *)iplimage,sizeof(IplImage)+iplimage->imageSize);

  /* Add it to the component's private list */
  mpf_component_list_buffer(component, buf);

  return buf;
}

/**
 * mpf_iplimage_rectify_imagedata:
 * @component: the #MpfComponent that owns the image.
 * @iplimage: an #IplImage to update.
 *
 * Updates an #IplImage internally after being copied.  Fixes the
 * internal pointer in the #IplImage header to point to the image
 * data that immediately follows it, as required by the GStreamer buffer
 * system.
 */
void
mpf_iplimage_rectify_imagedata(MpfComponent *component, IplImage *iplimage)
{
  iplimage->imageData = ((char *)iplimage + sizeof(IplImage));
}


/**
 * mpf_iplimage_make_writable_func:
 * @iplimage: an #IplImage to make writable
 *
 * Returns a pointer to a registered #IplImage that has been made
 * writable for the component.  If the component is the sole owner of
 * the image at this point, the same image is returned.  If there are
 * other users of the image, a copy will be made that is owned solely by
 * the current component.
 *
 * Returns: an #IplImage that may be the original or a copy, but is writable
 */
IplImage *
mpf_iplimage_make_writable_func(IplImage *iplimage)
{
  MpfComponent *component = mpf_component_get_curcomponent();
  GstBuffer *buf, *newbuf;

  /* See if there's a buffer associated with this image */
  buf = mpf_component_find_buffer(component,(gpointer)iplimage);

  /* If there is a buffer, we need to possibly make a copy of it */
  if (buf) {
    if (gst_buffer_is_writable(buf)) {
      return iplimage;
    } else {
      newbuf = gst_buffer_make_writable(buf);
      mpf_iplimage_rectify_imagedata(component, (IplImage*)GST_BUFFER_DATA(newbuf));
      mpf_component_list_buffer(component, newbuf);
      return (IplImage *)GST_BUFFER_DATA(newbuf);
    }

  /* Otherwise we just have a bare image, we're by definition the only owner */
  } else {
    return iplimage;
  }
}

/**
 * mpf_iplimage_make_from_img:
 * @refimg: an #IplImage to use as a template
 *
 * Constructs and return an #IplImage of the same format as the reference
 * image, registered with the component.
 *
 * Returns: an #IplImage sized and typed the same as the reference argument
 */
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

/**
 * mpf_iplimage_make_from_pad:
 * @name: name of the output pad to use as template
 *
 * Constructs an #IplImage suitable for sending over the given named pad.
 * Image type and size will be set appropriately for the pad's
 * negotiated format.
 *
 * Returns: an #IplImage typed for the given pad
 */
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



/**
 * mpf_iplimage_formatlist:
 * @type1: any of the CV_* format constants, and/or #MpfIplImageFormatList lists
 *
 * Builds a #uint64_t representing the collection of formats given as arguments,
 * including all the given OpenCV image type constants (e.g. CV_8UC3), as well as
 * any existing #MpfIplImageFormatList lists.
 *
 * Returns: a #MpfIplImageFormatList containing all the give types
 */
MpfIplImageFormatList
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


/**
 * mpf_iplimage_formatlist_to_caps:
 * @formatlist: a #MpfIplImageFormatList containing a set of OpenCV IplImage types
 *
 * Constructs a set of #GstCaps representing the union of all the types in the given
 * format list.
 *
 * Returns: a #GstCaps containing all given OpenCV IplImage types
 */
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
/**
 * mpf_iplimage_caps_to_formatlist:
 * @caps: a #GstCaps containing one or more OpenCV IplImage types
 *
 * Constructs a format list containing all the types listed in the given caps.
 *
 * Returns: a #MpfIplImageFormatList with all the types
 */
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
