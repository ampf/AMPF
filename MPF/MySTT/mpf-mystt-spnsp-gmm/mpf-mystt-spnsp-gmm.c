/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
//////////////////////////////////////////////////////////////////////
//
// Very fast speech/non-speech detector based on gaussian mixture
// models.  This component outputs a floating point speech and a
// non-speech score for each input feature. The scores are scaled log
// likelihoods, and can therefore be negative. If the speech score is
// larger than the non-speech score, the system believes the frame is
// speech. No temporal model or smoothing is applied -- those would
// be downstream components.
//
// Note that the input must be a stream of floating point
// features that match the models that are loaded into
// the component.
//
// The gmm routines are written in C++. We are not using the C++
// bindings to gstreamer since they're not finalized. Instead, we
// create some C wrappers for the C++ routines. The wrappers are in
// gmmbridge.cpp and gmmbridge.h, and operate on opaque pointers.
//
//
// Currently, everything is hardwired to use a particular
// set of features based on the ICSI Meeting Corpus. This also
// allows us to use our command line tools to process and debug
// .gmm files. Eventually, we'll move to a more general scheme
// where parameters are encoded in the .gmm file and caps negotiation
// based on them is performed.


#include <mpf/mpf-voidstar.h>
#include <gst/gst.h>

#include "gmmbridge.h"

#define COMPONENT_NAME "mpf-mystt-spnsp-gmm"
#define COMPONENT_DESC "Speech/non-speech detector"
#define COMPONENT_AUTH "Adam Janin <janin@icsi.berkeley.edu>"

#define SCORES_OUTPUT_CAPS				\
  "mpf-audio-features/speech-nonspeech-scores, "	\
  "nfeatures = (int) 2, "				\
  "samplerate = (int) 16000, "				\
  "windowtime = (int) 25, "				\
  "steptime = (int) 10"

#define FEATURES_INPUT_CAPS			\
  "mpf-audio-features/rasta, "			\
  "nfeatures = (int) 19, "			\
  "samplerate = (int) 16000, "			\
  "windowtime = (int) 25, "			\
  "steptime = (int) 10, "			\
  "modelorder = (int) 18, "			\
  "ceptralorder = (int) 19, "			\
  "dithered = (boolean) true, "			\
  "highpassfilter = (boolean) false "

struct component_private {
  cGMM* speech_gmm;
  cGMM* nonspeech_gmm;
  int   dim;		// Dimensionality
  int   ngaussians;
};

GstStaticCaps ScoresOutputStaticCaps = GST_STATIC_CAPS ( SCORES_OUTPUT_CAPS );
GstStaticCaps FeaturesInputStaticCaps = GST_STATIC_CAPS ( FEATURES_INPUT_CAPS );

//////////////////////////////////////////////////////////////////////
//
// MPF routines.
//

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  GstCaps* scores_output_caps;
  GstCaps* features_input_caps;

  // A lookup table is used for fast log. Currently disabled due to
  // issues with the fast log implementation on 64 bit architectures.

  initialize_table();	

  // Initialize input and output caps.
  // Eventually, we'll want to do negotiation based on which models
  // are being used.

  features_input_caps = gst_static_caps_get(&FeaturesInputStaticCaps);
  mpf_component_add_input_with_caps(NULL, "input", features_input_caps);
  
  scores_output_caps = gst_static_caps_get(&ScoresOutputStaticCaps);
  mpf_component_add_output_with_caps(NULL, "output", scores_output_caps);

  // Models are stored as plain files. Is there a standard
  // location to put these? Maybe /usr/share/mpf?

  mpf_add_param_string("speech", "Speech Model", "Speech GMM file",
		       "speech.gmm");

  mpf_add_param_string("nonspeech", "Nonspeech Model", "Nonspeech GMM file",
		       "nonspeech.gmm");
}

/* Component instance initialization and parameter handling. */
component_setup() {

  // Currently hardwired dimensionality and number of gaussians.
  // These will eventually be stored in the .gmm file and used
  // for caps negotiation. 

  mpf_private.dim = 19;
  mpf_private.ngaussians = 20;

  // Load GMMs

  mpf_private.speech_gmm = gmm_create(mpf_param_get_string("speech"),
				      mpf_private.ngaussians, mpf_private.dim);
  mpf_private.nonspeech_gmm = gmm_create(mpf_param_get_string("nonspeech"),
					 mpf_private.ngaussians, mpf_private.dim);
}

component_process() {
  GstBuffer *inbuf;
  GstBuffer *outbuf;
  int nread;
  int nframes;
  int frame;
  float* indata;
  float* outdata;
  
  inbuf = mpf_component_pull_buffer(NULL, "input");
  if (inbuf == NULL || (nread = GST_BUFFER_SIZE(inbuf)) <= 0) {
    // Empty buffer. Just return. Should this ever happen?
    return MPF_GOOD;
  }
  
  indata = (float*) GST_BUFFER_DATA(inbuf);

  // If the upstream component correctly generated data, there should
  // be one or more full frames, each frame with "dim" floating point
  // numbers.

  if (nread % mpf_private.dim*sizeof(float) != 0) {
    fprintf(stderr, "Got an incomplete frame of data in mpf-mystt-spnsp-gmm. An upstream component is probably set to the wrong dimensionality. Expected dimensionality is %d\n", mpf_private.dim);
    exit(EXIT_FAILURE);
  }

  nframes = nread / (mpf_private.dim*sizeof(float));

  // Allocate space for the output/

  outdata = (float*) malloc(sizeof(float)*2*nframes);
  if (!outdata) {
    perror("malloc of outdata failed in mpf-mystt-spnsp-gmm");
    exit(EXIT_FAILURE);
  }

  // Compute likelihoods on each frame.
  for (frame = 0; frame < nframes; frame++) {

    outdata[2*frame] = gmm_log_likelihood(mpf_private.speech_gmm, 
					  indata + frame*mpf_private.dim);

    outdata[2*frame+1] = gmm_log_likelihood(mpf_private.nonspeech_gmm, 
					    indata + frame*mpf_private.dim);
  }


  outbuf = mpf_voidstar_get_buffer(NULL, outdata);
  mpf_voidstar_set_length(outdata, sizeof(float)*2*nframes);
  mpf_voidstar_push("output", outdata);

  return MPF_GOOD;
}

/* Component instance finalization. */
component_teardown() {
  gmm_free(mpf_private.speech_gmm);
  gmm_free(mpf_private.nonspeech_gmm);
}
