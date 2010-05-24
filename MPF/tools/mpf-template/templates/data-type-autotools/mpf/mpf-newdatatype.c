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
 * SECTION: mpf-newdatatype
 * @short_description:
 */
#include <mpf/mpf-component.h>

#include "mpf-newdatatype.h"

GstStaticCaps mpf_newdatatype_caps = GST_STATIC_CAPS ( MPF_NEWDATATYPE_CAPS );

/**
 * mpf_newdatatype_add_input:
 * @name: name of the input pad to add
 *
 * Adds an input pad to the component of the given name
 */
void mpf_newdatatype_add_input(const gchar *name) {
  GstCaps *caps;

  caps = NULL;
  mpf_component_add_input_with_caps(NULL, name, caps);
}

/**
 * mpf_newdatatype_add_output:
 * @name: name of the output pad to add
 *
 * Adds an output pad to the component of the given name
 */
void mpf_newdatatype_add_output(const gchar *name) {
  GstCaps *caps;

  caps = NULL;
  mpf_component_add_output_with_caps(NULL, name, caps);
}

/**
 * mpf_newdatatype_get_buffer:
 * @component: a #MpfComponent to find the image in (or %NULL for current)
 * @newdatatype: the image to find the associated buffer for
 *
 * Returns a #GstBuffer that corresponds to the #Newdatatype passed to the
 * function.  This buffer either already exists and is returned as is,
 * or is created on demand and registered with the component appropriately.
 *
 * Returns: the #GstBuffer associated with the image
 */
GstBuffer * mpf_newdatatype_get_buffer(MpfComponent *component, Newdatatype *newdatatype) {
  GstBuffer *buf;

  /* If *comp is NULL, go pick it out of the thread state */
  if (component == NULL) {
    component = mpf_component_get_curcomponent();
  }

  /* If the buffer already exists for this newdatatype, return it */
  if ((buf = mpf_component_find_buffer(component, newdatatype)))
    return buf;

  /* Otherwise we need to make one */
  buf = gst_buffer_new();
  //gst_buffer_set_data(buf, (guint8 *)newdatatype, sizeof(Newdatatype) + newdatatype->imageSize);

  /* Add it to the component's private list */
  mpf_component_list_buffer(component, buf);

  return buf;
}


/**
 * mpf_newdatatype_rectify_imagedata:
 * @component: the #MpfComponent that owns the image
 * @newdatatype: an #Newdatatype to update
 *
 * Update a #Newdatatype internally after being copied.  Fixes the
 * internal pointer in the #Newdatatype header to point to the image
 * data that immediately follows it, as required by the GStreamer buffer
 * system.
 */
void mpf_newdatatype_rectify_imagedata(MpfComponent *component, Newdatatype *newdatatype) {
  //newdatatype->imageData = ((char *)newdatatype + sizeof(Newdatatype));
}


/**
 * mpf_newdatatype_make_writable_func:
 * @newdatatype: an #Newdatatype to make writable
 *
 * Return a pointer to a registered #Newdatatype that has been made
 * writable for the component.  If the component is the sole owner of
 * the image at this point, the same image is returned.  If there are
 * other users of the image, a copy will be made that is owned solely by
 * the current component.
 *
 * Returns: an #Newdatatype that may be the original or a copy, but is writable
 */
Newdatatype * mpf_newdatatype_make_writable_func(Newdatatype *newdatatype) {
  MpfComponent *component = mpf_component_get_curcomponent();
  GstBuffer *buf, *newbuf;

  /* See if there's a buffer associated with this image */
  buf = mpf_component_find_buffer(component, (gpointer)newdatatype);

  /* If there is a buffer, we need to possibly make a copy of it */
  if (buf) {
    if (gst_buffer_is_writable(buf)) {
      return newdatatype;
    } else {
      newbuf = gst_buffer_make_writable(buf);
      mpf_newdatatype_rectify_imagedata(component, (Newdatatype*)GST_BUFFER_DATA(newbuf));
      mpf_component_list_buffer(component, newbuf);
      return (Newdatatype *)GST_BUFFER_DATA(newbuf);
    }

  /* Otherwise we just have a bare image, we're by definition the only owner */
  } else {
    return newdatatype;
  }
}

/**
 * mpf_newdatatype_make_from_img:
 * @refimg: a #Newdatatype to use as a template
 *
 * Construct and return a #Newdatatype registered with the component.
 *
 * Returns: a #Newdatatype sized and typed the same as the reference argument
 */
Newdatatype * mpf_newdatatype_make_from_img(Newdatatype *refimg) {
  MpfComponent *component = mpf_component_get_curcomponent();
  GstBuffer *refbuf, *newbuf;
  Newdatatype *newimg;
  GstStructure *capstruct;
  int width, height, depth, channels;

  refbuf = mpf_component_find_buffer(component, refimg);
  newbuf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(refbuf));

  GST_BUFFER_TIMESTAMP (newbuf) = GST_BUFFER_TIMESTAMP (refbuf);
//  fprintf(stderr,"setting outbound buffer caps to %s\n",gst_caps_to_string(GST_BUFFER_CAPS (refbuf)));
  gst_buffer_set_caps(newbuf, GST_BUFFER_CAPS (refbuf));

  capstruct = gst_caps_get_structure(GST_BUFFER_CAPS (refbuf), 0);
  gst_structure_get_int(capstruct, "width", &width);
  gst_structure_get_int(capstruct, "height", &height);
  gst_structure_get_int(capstruct, "channels", &channels);
  depth = 8;

//  fprintf(stderr,"make_from() creating image of %dx%d, depth %d, %d channels\n",width,height,depth,channels);
  newimg = (Newdatatype *)GST_BUFFER_DATA(newbuf);
  //cvInitImageHeader(newimg, cvSize(width,height),SFV_DEPTH_8U,channels,0,4);
  //newimg->widthStep = width * depth / 8 * channels;
  //newimg->imageData = ((char *)newimg + sizeof(Newdatatype));

  mpf_component_list_buffer(component, newbuf);

  return newimg;
}

/**
 * mpf_newdatatype_make_from_pad:
 * @name: name of the output pad to use as template
 *
 * Construct an #Newdatatype suitable for sending over the given named pad.
 * Image type and size will be set appropriately for the pad's
 * negotiated format.
 *
 * Returns: a #Newdatatype typed for the given pad
 */
Newdatatype * mpf_newdatatype_make_from_pad(const gchar *padname) {
  MpfComponent *component = mpf_component_get_curcomponent();
  GstPad *pad;
  GstCaps *caps;
  GstBuffer *refbuf, *newbuf;
  Newdatatype *newimg;
  GstStructure *capstruct;
  int width, height, depth, channels;

  pad = gst_element_get_pad(GST_ELEMENT(component), padname);
  caps = gst_pad_get_allowed_caps(pad);

  newbuf = gst_buffer_new_and_alloc(GST_BUFFER_SIZE(refbuf));

  GST_BUFFER_TIMESTAMP (newbuf) = GST_BUFFER_TIMESTAMP (refbuf);
  fprintf(stderr,"setting outbound buffer caps to %s\n",gst_caps_to_string(GST_BUFFER_CAPS (refbuf)));
  gst_buffer_set_caps(newbuf, GST_BUFFER_CAPS (refbuf));

  capstruct = gst_caps_get_structure(GST_BUFFER_CAPS (refbuf), 0);
  gst_structure_get_int(capstruct, "width", &width);
  gst_structure_get_int(capstruct, "height", &height);
  gst_structure_get_int(capstruct, "channels", &channels);
  depth = 8;

  fprintf(stderr,"make_from() creating image of %dx%d, depth %d, %d channels\n",width,height,depth,channels);
  newimg = (Newdatatype *)GST_BUFFER_DATA(newbuf);
  //cvInitImageHeader(newimg, cvSize(width,height),SFV_DEPTH_8U,channels,0,4);
  //newimg->widthStep = width * depth / 8 * channels;
  //newimg->imageData = ((char *)newimg + sizeof(Newdatatype));

  mpf_component_list_buffer(component, newbuf);

  return newimg;
}

