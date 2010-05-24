/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* Start by including the headers for the two datatypes we need:
 * iplimage and objlist.
 */
#include <mpf/mpf-iplimage.h>
#include <mpf/mpf-objlist.h>

/* These defines must exist, and provide the template with the various
 * pieces of identification GStreamer needs.
*/
#define COMPONENT_NAME "cvhaar"
#define COMPONENT_DESC "OpenCV Haar object detection"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

/* This structure exists as "mpf_private" separately for each instance
 * of a component, used for storing parameters and persistant data.
 * This structure must exist even if it's empty, or the compiler will
 * error out.
 */
struct component_private {
  CvHaarClassifierCascade *cascade;
};

/* Here we include the template itself, which is responsible for making
 * this component as easy to code as it is.
 */
#include <mpf/mpf-componenttemplate.h>

/* The class_init function must register the inputs and outputs of the
 * component, as well as any parameters it might provide.  This is also
 * a good place to do any loading of common datasets.
 */
component_class_init() {
  mpf_iplimage_add_input("input", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_objlist_add_output("output");
}

/* The component_init function is called for each instance of the
 * component, and is where we are going to load the Haar cascade into
 * our private structure.
 */
component_init() {
  mpf_private.cascade = (CvHaarClassifierCascade *)
  cvLoad("haarcascade_frontalface_alt.xml", 0, 0, 0);
}

/* The main process function is where the real work happens.  It is
 * called once per "frame", at which point all the buffers of the same
 * timestamp from each of the element's inputs are available.
 */
component_process() {
  struct component_private *priv = &MPF_COMPONENT_PRIVATE(component);
  CvMemStorage *storage;
  CvSeq *faces;
  int i;
  MpfObjectList *facelist;

/* The first thing we do is pull the image we want to work on from our
 * input pad.  We are using the data-type specific function for this,
 * which means its return is indeed an IplImage *.
 */
  IplImage *src = mpf_iplimage_pull("input");

/* Here we perform our actual processing, which is a call out to OpenCV.
 */
  storage = cvCreateMemStorage(0);
  faces = cvHaarDetectObjects (src, priv->cascade, storage, 1.1, 2,
  CV_HAAR_DO_CANNY_PRUNING, cvSize(30,30));

/* As we are now done with the image data itself, and we are not going
 * to be pushing it further down the pipeline, we must release our
 * reference to it.  If we are the only user of the image, it will be
 * freed from memory.
 */
  mpf_iplimage_unref(src);

/* The output of this component is an object list, in this case
 * containing the locations and sizes of the faces found in the image.
 * We use a data-type specific allocation routine, which gives us the
 * structure of the right size.
 */
  facelist = mpf_objlist_new(faces->total);

/* Looping through the OpenCV-provided list of found objects, we
 * transliterate it into the objlist format.
 */
  for (i=0;i< faces->total; i++) {
    CvRect *r = (CvRect *)cvGetSeqElem(faces, i);
    facelist->list[i].type = MPF_OBJLIST_TYPE_FACE;
    facelist->list[i].x = r->x;
    facelist->list[i].y = r->y;
    facelist->list[i].w = r->width;
    facelist->list[i].h = r->height;
    facelist->list[i].quality = 10;
    facelist->list[i].uid = -1;
    facelist->list[i].name = g_strdup_printf("Face");
  }

/* And finally, we push the face list out to whatever the next element
 * is in the chain.  The reference we "owned" by creating the objlist is
 * implicitly transfered over to the next element.
 */
  mpf_objlist_push("output", facelist);

  return MPF_GOOD;
}
