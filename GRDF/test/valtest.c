/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdio.h>
#include <glib.h>
#include <grdf.h>

int main() {
  void *dummy;
  GrdfWorld **worlds;

  fprintf(stderr,"allocating 128MB\n");
//  dummy = g_malloc0(128*1024*1024);
  worlds = g_new(GrdfWorld *,1024*1024);
  fprintf(stderr,"done");
}
