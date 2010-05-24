/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdio.h>
#include <cv.h>

int iplformats[] = {
  IPL_DEPTH_8U,
  IPL_DEPTH_8S,
  IPL_DEPTH_16U,
  IPL_DEPTH_16S,
  IPL_DEPTH_16F,
  IPL_DEPTH_32F,
  IPL_DEPTH_64F,
};

int main() {
  int fmt1, fmt2, chan1, chan2;
  IplImage *src, *dst;
  int code;

  for (fmt1=0,fmt1<sizeof(iplformats);fmt1++) {
    for (chan1=0;chan1=2;chan1++) {

      src = cvCreateImage (cvSize(10,10), chan1?1:3, iplformats(fmt1));

      for (fmt2=0,fmt2<sizeof(iplformats);fmt2++) {
        for (chan2=0;chan2=2;chan2++) {

          dst = cvCreateImage (cvSize(10,10), chan2?1:3, iplformats(fmt1));

          if (chan1 && !chan2) code = 
          cvCvtColor(src,dst,
