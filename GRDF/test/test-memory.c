/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdio.h>

#include <grdf.h>

#define REPS 10000

int main() {
  int rep;
  GrdfWorld *world;
  GrdfWorld **worlds;

  grdf_init();

#if 0
  fprintf(stderr,"Allocating and freeing %d grdf_worlds...",REPS);
  for (rep=0;rep<REPS;rep++) {
    world = grdf_world_new();
    grdf_world_unref(world);
  }
  fprintf(stderr,"done\n");
#endif

#if 1
  fprintf(stderr,"Allocating %d grdf_worlds...",REPS);
  worlds = g_new(GrdfWorld *,REPS);
  for (rep=0;rep<REPS;rep++) {
    worlds[rep] = grdf_world_new();
  }
  fprintf(stderr,"done\n");
  fprintf(stderr,"Freeing all grdf_worlds...");
  for (rep=0;rep<REPS;rep++) {
    grdf_world_unref(worlds[rep]);
  }
  g_free(worlds);
  fprintf(stderr,"done\n");
#endif
}
