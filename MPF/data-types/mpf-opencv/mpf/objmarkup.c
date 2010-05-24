/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>
#include <mpf/mpf-objlist.h>

#define COMPONENT_NAME "objmarkup"
#define COMPONENT_DESC "Object-list markup"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
  CvFont font;
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_iplimage_add_input("imgsink", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_objlist_add_input("objsink");
  mpf_iplimage_add_output("src", MPF_IPLIMAGE_FORMAT_ANY);
}

component_init() {
  cvInitFont(&mpf_private.font, CV_FONT_HERSHEY_PLAIN, 1.25, 1.25, 0.0, 2, 8);
}

component_process() {
  int i;
  int r,g,b;

  IplImage *src = mpf_iplimage_pull("imgsink");
  src = mpf_iplimage_make_writable(src);

  MpfObjectList *facelist = mpf_objlist_pull("objsink");
  for (i=0;i<facelist->count; i++) {
    CvPoint pt1,pt2;
    pt1.x = facelist->list[i].x;
    pt1.y = facelist->list[i].y;
    pt2.x = facelist->list[i].x + facelist->list[i].w;
    pt2.y = facelist->list[i].y + facelist->list[i].h;

//    r = 120 - (facelist->list[i].quality * 12);
//    g = 135 + (facelist->list[i].quality * 12);
//    b = 120 - (facelist->list[i].quality * 12);
    r = 0;
    g = facelist->list[i].quality * 25;
    b = 0;

    cvRectangle(src,pt1,pt2,CV_RGB(r,g,b),3,8,0);
//    fprintf(stderr,"** objmarkup: have face at %d,%d-%d,%d\n",pt1.x,pt1.y,pt2.x,pt2.y);
    if (facelist->list[i].name != NULL) {
      cvPutText(src, facelist->list[i].name, cvPoint (pt1.x+4, pt2.y-4), &mpf_private.font,
  CV_RGB(r,g,b));
    }
  }

  mpf_iplimage_push("src", src);

  return MPF_GOOD;
}
