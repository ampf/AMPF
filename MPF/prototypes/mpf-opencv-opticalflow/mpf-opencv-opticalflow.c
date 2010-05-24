/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-iplimage.h>
#include <mpf/mpf-rdf.h>

#define COMPONENT_NAME "mpf-opencv-opticalflow"
#define COMPONENT_DESC "Optical Flow using Block Matching Algorithm"
#define COMPONENT_AUTH "Appscio, Inc. <info@appscio.com>"

/* Reference field as mpf_private.field */
struct component_private {
  /* previous grayscale image */
  IplImage *previous_image;
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_iplimage_add_input("input", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_iplimage_add_output("velocity_x", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_iplimage_add_output("velocity_y", MPF_IPLIMAGE_FORMAT_ANY);
  mpf_rdf_add_output("average_flow");

  // OpenCV bug prevents block_size > 1 from working
  //mpf_add_param_int("block_size", "BlockSize", "Block Size", 1, 32767, 16);
  mpf_add_param_int("block-size", "BlockSize", "Block Size", 1, 32767, 1);
  mpf_add_param_int("shift-size", "ShiftSize", "Shift Size", 1, 32767, 1);
  //mpf_add_param_int("max-range", "MaxRange", "Max Range", 1, 32767, 10);
  mpf_add_param_int("max-range", "MaxRange", "Max Range", 1, 32767, 5);
}

/* Calculates the size of the output IplImage which contains */
/* the optical flow data */
/* workaround for OpenCV bug, hardcode output image size to match input */
//static CvSize get_vel_size(IplImage *img, CvSize block_size,
//    CvSize shift_size) {
//
//  int x = floor((img->width - block_size.width) / shift_size.width);
//  int y = floor((img->height - block_size.height) / shift_size.height);
//  return cvSize(x, y);
//}

component_init() {
  mpf_private.previous_image = NULL;
}

component_process() {
  int bs = mpf_param_get_int("block-size");
  if (bs == 0) {
    bs = 1;
  }
  int ss = mpf_param_get_int("shift-size");
  if (ss == 0) {
    ss = 1;
  }
  int mr = mpf_param_get_int("max-range");
  if (mr == 0) {
    mr = 5;
  }

  CvSize block_size = cvSize(bs, bs);
  CvSize shift_size = cvSize(ss, ss);
  CvSize max_range = cvSize(mr, mr);
  int use_previous = 0;

  /* previous grayscale image */
  IplImage *gray1 = mpf_private.previous_image;

  /* get current image */
  IplImage *img2 = mpf_iplimage_pull("input");
  /* convert current image to grayscale */
  IplImage *gray2 = cvCreateImage(cvGetSize(img2), IPL_DEPTH_8U, 1);
  cvCvtColor(img2, gray2, CV_BGR2GRAY);

  /* save the current grayscale image for next time */
  mpf_private.previous_image = gray2;

  /* no previous image, this is the first */
  if (gray1 == NULL) {
    mpf_iplimage_unref(img2);
    mpf_iplimage_unref(gray2);
    return MPF_GOOD;
  }

  /* workaround for OpenCV bug, hardcode output image size to match input */
  //CvSize vel_size = get_vel_size(gray1, block_size, shift_size);
  CvSize vel_size = cvGetSize(gray1);
  /* create output images */
  IplImage *vel_x = cvCreateImage(vel_size, IPL_DEPTH_32F, 1);
  IplImage *vel_y = cvCreateImage(vel_size, IPL_DEPTH_32F, 1);

/* This is the bogus check in opencv */
//fprintf(stderr, "(unsigned)(vel_x->width*block_size.width - gray1->width): %d >= %d: (unsigned)block_size.width\n", (unsigned)(vel_x->width*block_size.width - gray1->width), (unsigned)block_size.width);
//fprintf(stderr, "(unsigned)(vel_x->height*block_size.height - gray1->height): %d >= %d: (unsigned)block_size.height\n", (unsigned)(vel_x->height*block_size.height - gray1->height), (unsigned)block_size.height);

  /* Calculate the optical flow using the Block Method */
  cvCalcOpticalFlowBM(gray1, gray2, block_size,
      shift_size, max_range, use_previous, vel_x, vel_y);

  /* Calculate running average flow vector (2D) */
  /* This takes more floating point operations but ensures we don't overflow */
  float x, y;
  float average_x = 0.0;
  float average_y = 0.0;
  float count = 0.0;
  int i, j;
  /* vel_x and vel_y are the same size so we can use the same loop */
  for (i = 0; i < vel_x->height; i++) {
    for (j = 0; j < vel_x->width; j++) {
      x = ((float *)(vel_x->imageData + i*vel_x->widthStep))[j];
      y = ((float *)(vel_y->imageData + i*vel_y->widthStep))[j];
      count = count + 1.0;
      average_x = (average_x * ((count - 1) / count)) + (x / count);
      average_y = (average_y * ((count - 1) / count)) + (y / count);
    }
  }
fprintf(stderr, "average_x: %f\n", average_x);
fprintf(stderr, "average_y: %f\n", average_y);

  GrdfGraph *graph = mpf_rdf_new();
  GrdfNode *node = grdf_node_anon_new(graph, "average_flow");
  grdf_stmt_new_nuu(graph, node, "isA", "appscio:Vector2DF");
  grdf_stmt_new_nuf(graph, node, "appscio:Vector2DX", average_x);
  grdf_stmt_new_nuf(graph, node, "appscio:Vector2DY", average_y);

  mpf_iplimage_unref(gray1);
  mpf_iplimage_unref(img2);
  mpf_iplimage_unref(gray2);

  mpf_iplimage_push("velocity_x", vel_x);
  mpf_iplimage_push("velocity_y", vel_y);
  mpf_rdf_push("average_flow", graph);

  return MPF_GOOD;
}

