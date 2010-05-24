/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
//////////////////////////////////////////////////////////////////////
//
// File: spnsp_test.c
// Author: Adam Janin
//         Copyright 2009 International Computer Science Institute
//
// (Insert standard Appscio licensing here)
//
// Command line version of the speech/non-speech detector.
//
// This is mostly used for debugging, and may not be fully functional.
//
// Note that we are not using the C++ bindings to gstreamer since
// they're not finalized. Instead, we create some C wrappers for the
// C++ routines.
//

#include <stdio.h>
#include <stdlib.h>

#include "gmmbridge.h"

//////////////////////////////////////////////////////////////////////
//
// Prototypes
//

void usage();
void process_spnsp(float*, int nread);

struct _mpf_private {
  cGMM* speech_gmm;
  cGMM* nonspeech_gmm;
  int   dim;
  int   ngaussians;
} mpf_private;

int main(int argc, char** argv) {
  float* inbuf;
  int nread, nvecs;
  
  if (argc != 5) {
    usage();
  }

  initialize_table();

  if (sscanf(argv[3], "%d", &mpf_private.ngaussians) != 1) {
    fprintf(stderr, "Couldn't read number of gaussians\n");
    usage();
  }

  if (sscanf(argv[4], "%d", &mpf_private.dim) != 1) {
    fprintf(stderr, "Couldn't read dimensionality\n");
    usage();
  }
  
  mpf_private.speech_gmm = gmm_create(argv[1], mpf_private.ngaussians, mpf_private.dim);
  mpf_private.nonspeech_gmm = gmm_create(argv[2], mpf_private.ngaussians, mpf_private.dim);

  // "Main" loop.
  
  // Number of feature vectors receieved.  This should be randomized
  // for testing.
  nvecs = 21;
  inbuf = (float*) malloc(sizeof(float)*nvecs*mpf_private.dim);
  if (!inbuf) {
    perror("Couldn't allocate input buffer");
    exit(EXIT_FAILURE);
  }
  
  do {
    nread = fread(inbuf, 1, sizeof(float)*mpf_private.dim*nvecs, stdin);
    process_spnsp(inbuf, nread);
  } while (nread == sizeof(float)*mpf_private.dim*nvecs);

  free(inbuf);
  
  gmm_free(mpf_private.speech_gmm);
  gmm_free(mpf_private.nonspeech_gmm);
  
  return(EXIT_SUCCESS);
}

void usage() {
  fprintf(stderr, "Usage: spnsp speech.gmm nonspeech.gmm ngaussians dimensionality < infile.features > outfile.loglikelihoods\n");
  exit(EXIT_FAILURE);
}

  
//////////////////////////////////////////////////////////////////////
//
// nread is number of BYTES read.
//
  
void process_spnsp(float* inbuf, int nread) {
  int frame, nframes;
  float speech_score, nonspeech_score;
  
  if (nread % mpf_private.dim*sizeof(float) != 0) {
    fprintf(stderr, "Got an incomplete frame of data in spnsp. An upstream component is probably set to the wrong dimensionality. Expected dimensionality is %d\n", mpf_private.dim);
    exit(EXIT_FAILURE);
  }
  nframes = nread / (mpf_private.dim*sizeof(float));
  
  //  fprintf(stderr, "Read %d bytes = %d frames\n", nread, nframes);
  
  for (frame = 0; frame < nframes; frame++) {
    speech_score = gmm_log_likelihood(mpf_private.speech_gmm, inbuf + frame*mpf_private.dim);
    nonspeech_score = gmm_log_likelihood(mpf_private.nonspeech_gmm, inbuf + frame*mpf_private.dim);
    printf("%f %f\n", speech_score, nonspeech_score);
  }
}
