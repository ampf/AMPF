/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-objlist.h>
#include <math.h>

#define COMPONENT_NAME "objcorrelate"
#define COMPONENT_DESC "Object-list correlation"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
  MpfObjectList *prev;
  guint64 next_uid;
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_objlist_add_input("input");
  mpf_objlist_add_output("output");
}

component_init() {
  // Create a dummy previous list
  mpf_private.prev = mpf_objlist_new(0);

  mpf_private.next_uid = 42;
}

#if 0
component_process() {
  MpfObjectList *in, *out, *prev, *temp;
  guint64 prevmask = 0LL;
  int i,j;
  int x1,y1,d1,x2,y2,d2;
  float dist,mindist;
  int bestmatch;
  int outcount;

  // Pull in the new object data
  in = mpf_objlist_pull("input");

  // Allocate a temporary list that's big enough for both
  temp = mpf_objlist_new(in->count + mpf_private.prev->count);

  prev = mpf_private.prev;

  // For each entry in the current list...
  for (i=0;i<in->count;i++) {
    // Calculate the center and "diameter"
    x1 = in->list[i].x + in->list[i].w/2;
    y1 = in->list[i].y + in->list[i].h/2;
    d1 = in->list[i].w/2 + in->list[i].h/2;

    mindist = 1000000;
    bestmatch = -1;

    // Check each entry in the input list
    for (j=0;j<prev->count;j++) {

      // Calculate the center and "diameter"
      x2 = prev->list[i].x + prev->list[i].w/2;
      y2 = prev->list[i].y + prev->list[i].h/2;
      d2 = prev->list[i].w/2 + prev->list[i].h/2;

      // Calculate distance between the two
      dist = sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));
      if (dist < mindist) {
        mindist = dist;
        bestmatch = j;
      }
    }

    // Copy the new entry into the temp list
    memcpy(&(temp->list[i]),&(in->list[i]),sizeof(MpfObjectListEntry));

    // If it's close enough, correlate
    if (mindist < 15) {
      // Copy previous iteration's UID and update the quality metric
      temp->list[i].uid = prev->list[bestmatch].uid;
      if (prev->list[bestmatch].quality != 10)
        temp->list[i].quality = prev->list[bestmatch].quality + 1;

      // Mark the previous entry as 'correlated'
      prevmask |= (1<<j);

    // Otherwise, we start a new entry at quality 1
    } else {
      temp->list[i].uid = mpf_private.next_uid;
      mpf_private.next_uid++;
      temp->list[i].quality = 1;
    }
  }
  outcount = in->count;

  // Now go through the previous entries and count uncorrelated
  for (i=0;i<prev->count;i++) {
    // Skip it if it's been correlated
    if (prevmask & (1<<i)) continue;
    // Count it
    outcount++;
  }

  // Allocate the new out list
  out = mpf_objlist_new(outcount);

  // Copy all of the temp incoming entries
  memcpy(&(out->list[0]),&(temp->list[0]),sizeof(MpfObjectListEntry) * in->count);

  // Copy the uncorrelated entries to the end of the list
  outcount = in->count;
  for (i=0;i<prev->count;i++) {
    // Skip it if it's been correlated
    if (prevmask & (1<<i)) continue;

    // Copy previous entry into temp
    memcpy(&(out->list[outcount]),&(prev->list[i]),sizeof(MpfObjectListEntry));

    // Reduce the quality
    out->list[j].quality--;

    // Increment output count
    outcount++;
  }

  mpf_objlist_unref(in);
  mpf_objlist_unref(temp);
  mpf_objlist_unref(prev);

  mpf_private.prev = out;
  mpf_objlist_ref(mpf_private.prev);

  mpf_objlist_push("output", out);
}

#else

component_process() {
  MpfObjectList *in, *out, *prev;
  int i,j;
  int x1,y1,d1, x2,y2,d2;
  float dist, mindist;
  int bestmatch;

  // Pull in the new object data and get a convenient pointer to the previous
  in = mpf_objlist_pull("input");
  prev = mpf_private.prev;

  fprintf(stderr,"\n** Correlate: input list has %d entries, previous has %d\n",
    in->count,prev->count);

  // Loop through each input entry and see if there's a match in the prev
  for (i=0;i<in->count;i++) {
    // Calculate the center and "diameter"
    x1 = in->list[i].x + in->list[i].w/2;
    y1 = in->list[i].y + in->list[i].h/2;
    d1 = in->list[i].w/2 + in->list[i].h/2;

    fprintf(stderr,"** Correlate: checking current obj %d,%d-%d\n",x1,y1,d1);

    mindist = 1000000;
    bestmatch = -1;

    // Check each entry in the input list
    for (j=0;j<prev->count;j++) {

      // Calculate the center and "diameter"
      x2 = prev->list[i].x + prev->list[i].w/2;
      y2 = prev->list[i].y + prev->list[i].h/2;
      d2 = prev->list[i].w/2 + prev->list[i].h/2;

      // Calculate distance between the two
      dist = sqrt(((x2-x1)*(x2-x1)) + ((y2-y1)*(y2-y1)));

      fprintf(stderr,"** Correlate:	against prev %d,%d-%d, dist is %.0f\n",x1,y1,d1,dist);

      if (dist < mindist) {
        mindist = dist;
        bestmatch = j;
      }
    }
  }

  // temp
  out = in;

  // Release the previous list and store the output for next frame
  mpf_objlist_unref(prev);
  mpf_objlist_ref(out);
  mpf_private.prev = out;

  mpf_objlist_push("output", out);

  return MPF_GOOD;
}


#endif
