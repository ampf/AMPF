/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
//////////////////////////////////////////////////////////////////////
//
// Reads an audio stream, writes "rasta" features.
//
// This is a modified version of rasta.c from the rasta distribution
// ported to mpf and written by Adam Janin <janin@icsi.berkeley.edu>
//
// Note that this implementation hasn't been highly
// optimized for buffering and windowing because I plan
// to write a separate windowing element eventually.
//
// The parameter handling is still very "command line" oriented.
// We'll convert to gstreamer conventions as necessary.
//
// Note that this code links with the standard librasta.a library
// from the rasta distribution. This puts some constraints on
// interface, since the library was written with command line
// in mind. For example, argument handling is through argc/argv.
//
// Currently hard-wired to read 16 bit integer PCM, 16 kHz,
// little-endian audio and write 4 byte little endian floats.
// Analysis is over 25ms windows, 10ms step size, 19 features
// (18 PLP + energy).
//
// Note on endianness:
//
//  The original rasta code always reads and writes binary big-endian
//  data. This can be changed using command line flags. Since we're
//  typically on a little-endian machine, we use the flags (which
//  results in no physical swapping if you're on a little endian
//  machine).
//
//  All the endian logic should probably be in gstreamer, not this
//  component.
//
//
// TODO:
//   Handle caps negotiation. Right not, it's hard wired for
//   particular choices (the models we had on hand).

#include <assert.h>

#include <rasta.h>
#include <mpf/mpf-voidstar.h>

//////////////////////////////////////////////////////////////////////
//
// Defines
//

#define COMPONENT_NAME "mpf-mystt-feature-gen-rasta"
#define COMPONENT_DESC "Compute RASTA features on audio input"
#define COMPONENT_AUTH "Adam Janin, International Computer Science Institute <janin@icsi.berkeley.edu>"

#define AUDIO_SINK_CAPS				\
  "audio/x-raw-int, "				\
  "width = (int) 16, "				\
  "depth = (int) 16, "				\
  "signed = (boolean) true, "			\
  "endianness = (int) 1234, "			\
  "channels = (int) 1, "			\
  "rate = (int) 16000"

#define FEATURES_SOURCE_CAPS			\
  "mpf-audio-features/rasta, "			\
  "nfeatures = (int) 19, "			\
  "samplerate = (int) 16000, "			\
  "windowtime = (int) 25, "			\
  "steptime = (int) 10, "			\
  "modelorder = (int) 18, "			\
  "ceptralorder = (int) 19, "			\
  "dithered = (boolean) true, "			\
  "highpassfilter = (boolean) false "

//////////////////////////////////////////////////////////////////////
//
// Types
//

struct component_private {

  // Command line parameters
  struct param runparam;

  // History. Used for experimental noise level estimation.
  // Not well tested -- I recommend not using history.
  struct fhistory history;	
  
  // Data left over from previous reads. At most window size floats.
  float* leftover;
  int nleftover;	// Number of actual valid points in leftover.
  
  struct fvec* window;	// Hamming window parameters
};

//////////////////////////////////////////////////////////////////////
//
// Prototypes
//

static void reset_state(struct param *runparam, struct fhistory *history);
static void local_endian_swap(short *sbuf, int nshorts);
static void process_rasta_one_frame(float* indata, float* outdata,
				    struct param *runparam, struct fhistory *history, struct fvec* window);

//////////////////////////////////////////////////////////////////////
//
// Globals
//

GstStaticCaps AudioSinkStaticCaps = GST_STATIC_CAPS ( AUDIO_SINK_CAPS );
GstStaticCaps FeaturesSourceStaticCaps = GST_STATIC_CAPS ( FEATURES_SOURCE_CAPS );

//////////////////////////////////////////////////////////////////////
//
// Local functions.
//

static void reset_state(struct param *runparam, struct fhistory *history) {
    /* reset state in history structure, rasta filter, and
       noise estimation. */

    history->normInit = TRUE;
    if (runparam->history == TRUE) {
	load_history(history, runparam);
    }

    history->eos = TRUE;	/* causes rasta_filt to reset */
    rasta_filt(history, runparam, (struct fvec*) NULL);

    if ((runparam->jrasta == TRUE ) &&
	(runparam->cJah == FALSE)) { 
	comp_noisepower(history, runparam, (struct fvec*) NULL);
    }

    history->eos = FALSE;
}



// Ugly historic endian issues.
//
// The original rasta always reads and writes big-endian unless
// overridden by command line flags.  This should probably be changed
// to always read and write native endianness, and have endian
// switchers in the gstream as needed.

//
// In vanilla rasta, this is compiled into the library.

static void endian_swap_short(short *sbuf, int nshorts) {
    /* Swap the bytes in a buffer full of shorts if this machine 
       is not big-endian */
    int i, x;
    for (i=0; i<nshorts; ++i) {
	x = *sbuf;
	*sbuf++ = ((x & 0xFF) << 8) + ((x >> 8) & 0xFF);
    }
}

static void endian_swap_int(int *buf, int nints) {
    /* Swap the bytes in a buffer full of ints if this machine 
       is not big-endian */
    int i, x;
    assert(sizeof(int)==4);
    for (i=0; i<nints; ++i) {
	x = *buf;
	*buf++ = ((x & 0xFFL) << 24L) + ((x & 0xFF00L) << 8L) \
	           + ((x & 0xFF0000L) >> 8L) + ((x >> 24L) & 0xFF);
    }
}

static void endian_swap_long(long *buf, int nints) {
    /* Swap the bytes in a buffer full of longs if this machine 
       is not big-endian */
    int i;
    long x;
    assert(sizeof(long)==4);
    for (i=0; i<nints; ++i) {
	x = *buf;
	*buf++ = ((x & 0xFFL) << 24L) + ((x & 0xFF00L) << 8L) \
	           + ((x & 0xFF0000L) >> 8L) + ((x >> 24L) & 0xFF);
    }
}

static void endian_swap_float(float *buf, int nints) {
    /* Swap the bytes in a buffer full of floats if this machine 
       is not big-endian */
    assert(sizeof(float)==4);
    endian_swap_int((int*)buf, nints);
}

#ifdef WORDS_BIGENDIAN
#define endian_fix_short(a,b,swap)	if(swap) endian_swap_short(a,b)
#define endian_fix_int(a,b,swap)	if(swap) endian_swap_int(a,b)
#define endian_fix_long(a,b,swap)	if(swap) endian_swap_long(a,b)
#define endian_fix_float(a,b,swap)	if(swap) endian_swap_float(a,b)
#else /* !WORDS_BIGENDIAN */
#define endian_fix_short(a,b,swap)	if(!swap) endian_swap_short(a,b)
#define endian_fix_int(a,b,swap)	if(!swap) endian_swap_int(a,b)
#define endian_fix_long(a,b,swap)	if(!swap) endian_swap_long(a,b)
#define endian_fix_float(a,b,swap)	if(!swap) endian_swap_float(a,b)
#endif /* !WORDS_BIGENDIAN */

//////////////////////////////////////////////////////////////////////
//
// Given exactly one window of data, compute features and
// store in output buffer.
//
// Computes high pass filter if requested.
// Applies hamming window.
// Computes rasta.
//

static void process_rasta_one_frame(float* indata, float* outdata,
				    struct param *runparam, struct fhistory *history, struct fvec* window) {
  int ii;
  static struct fvec* frame = NULL;
  struct fvec* cepstrum;
  
  if (frame == NULL) {
    frame = alloc_fvec(runparam->winpts);
  }  

  // Don't clobber the indata
  for (ii = 0; ii < frame->length; ii++) {
    frame->values[ii] = indata[ii];
  }
  
  // High pass filter if requested.
  if (runparam->HPfilter == TRUE) {
    fvec_HPfilter(history, runparam, frame);
  }
  
  // Apply hamming window.
  for (ii = 0; ii < runparam->winpts; ii++) {
    frame->values[ii] = frame->values[ii] * window->values[ii];
  }
  
  // Compute the features.
  
  cepstrum = rastaplp( history, runparam, frame );
  
  // Output
  endian_fix_float(cepstrum->values, cepstrum->length, runparam->outswapbytes);

  for (ii = 0; ii < cepstrum->length; ii++) {
    outdata[ii] = cepstrum->values[ii];
  }
}

//////////////////////////////////////////////////////////////////////
//
// MPF "callbacks"
//

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  GstCaps* audio_sink_caps;
  GstCaps* features_source_caps;

  audio_sink_caps = gst_static_caps_get(&AudioSinkStaticCaps);
  mpf_component_add_input_with_caps(NULL, "input", audio_sink_caps);

  features_source_caps = gst_static_caps_get(&FeaturesSourceStaticCaps);
  mpf_component_add_output_with_caps(NULL, "output", features_source_caps);

  /* Add parameters (name, nickname, description, min, max, default) */
  // mpf_add_param_int("object-count", "Object Count", "Number of objects",
  //   0, 32, 3);
}

//////////////////////////////////////////////////////////////////////
//
// Initialize arguments
//

component_setup() {
    char *argv[] = { 
    "mpf-mystt-feature-gen-rasta",
    "-O",	// Online. Required.
    "-M",	// dither. Optional.
//    "-F",	// high pass filter. Optional.
     "-T",	// Swap input bytes endianess. See note on endianness above.
     "-U",	// Swap output byte endianess. See note on endianness above.
    "-w 25",	// Window size in ms
    "-s 10",	// Step size in ms
    "-S 16000",	// Sampling frequency
    "-m 18",	// Model order
  };
  
  int argc = sizeof(argv)/sizeof(char*);

  //
  // Set up runparam based on command line
  //
  
  get_comline(&mpf_private.runparam, argc,argv);

  //
  // Check that params are valid. Print message and exit otherwise.
  //

  check_args( &mpf_private.runparam );

  // If the debugging is on, show the command-line arguments

  if (mpf_private.runparam.debug == TRUE) {
      show_args( &mpf_private.runparam );
  }

  if (mpf_private.runparam.infname != NULL) {
    fprintf(stderr, "mpf-mystt-feature-gen-rasta ignoring input file specification. Using input stream\n");
  }
  mpf_private.runparam.infname = "-";

  if (mpf_private.runparam.outfname != NULL) {
    fprintf(stderr, "mpf-mystt-feature-gen-rasta ignoring output file specification. Using output stream\n");
  }
  mpf_private.runparam.outfname = "-";

  mpf_private.history.eof = FALSE;

  // Compute other parameters based on existing parameters.
  // (e.g. winpts from sampfreq and winsize)

  init_param(0, &mpf_private.runparam);
  
  reset_state(&mpf_private.runparam, &mpf_private.history);

  // There are a few parameters that I've trimmed off to make the code
  // smaller and easier to understand. They should all almost
  // certainly be their own components if they're needed.

  if (mpf_private.runparam.padInput == TRUE) {
    fprintf(stderr, "padInput is not supported\n");
    exit(-1);
  }
  if (mpf_private.runparam.deltaorder != 0) {
    fprintf(stderr, "deltas are not supported\n");
    exit(-1);
  }
  if (mpf_private.runparam.strut_mode) {
    fprintf(stderr, "Strut mode not supported\n");
    exit(-1);
  }

  // Allocate space for "left over" data from previous reads for windowing.
  mpf_private.leftover = (float*) malloc(sizeof(float)*mpf_private.runparam.winpts);
  if (!mpf_private.leftover) {
    perror("malloc of \"leftover\" failed in rasta");
    exit(-1);
  }
  mpf_private.nleftover = 0;

  // Compute and save hamming window parameters.
  mpf_private.window = get_win(&mpf_private.runparam, mpf_private.runparam.winpts);
}

component_process() {
  int nnewdata;
  GstBuffer *inbuf;
  GstBuffer *outbuf;
  short* newdata;
  float* alldata;
  float* outdata;
  int ii, ff;
  int nframes;
  int winpts;
  int steppts;
  int nleftover_new;
  
  inbuf = mpf_component_pull_buffer(NULL, "input");

  if (inbuf == NULL || (nnewdata = (GST_BUFFER_SIZE(inbuf)/sizeof(short))) <= 0) {
    // Empty buffer. Just return. Should this ever happen?
    return MPF_GOOD;
  }
  
  newdata = (short*) GST_BUFFER_DATA(inbuf);

  // Bytes swap as needed.
  
  endian_fix_short(newdata, nnewdata, mpf_private.runparam.inswapbytes);

  // Used everywhere, so save some typing.
  
  winpts = mpf_private.runparam.winpts;
  steppts = mpf_private.runparam.steppts;
  
  // If we don't have enough for a full window, just append it to
  // leftover and return.
  
  if (nnewdata + mpf_private.nleftover < winpts) {
    for (ii = 0; ii < nnewdata; ii++) {
      mpf_private.leftover[mpf_private.nleftover+ii] = (float) newdata[ii];
    }
    mpf_private.nleftover += nnewdata;
    return MPF_GOOD;
  }
  
  // If we get here, we have enough for at least one frame.

  nframes = 1 + (int) ((nnewdata+mpf_private.nleftover-winpts)/steppts);

  // Create a buffer for the left over data plus the new data. Inefficient.

  alldata = (float*) malloc(sizeof(float)*(nnewdata+mpf_private.nleftover));
  if (!alldata) {
    perror("malloc for data failed in rasta");
    exit(-1);
  }

  // Create space for all output.

  outdata = (float*) malloc(sizeof(float)*nframes*mpf_private.runparam.nout);
  if (!outdata) {
    perror("malloc for output data failed in rasta");
    exit(-1);
  }

  // Copy any leftover plus the new data into alldata.

  for (ii = 0; ii < mpf_private.nleftover; ii++) {
    alldata[ii] = mpf_private.leftover[ii];
  }
  for (; ii < nnewdata+mpf_private.nleftover; ii++) {
    alldata[ii] = (float) newdata[ii-mpf_private.nleftover];
  }

  // Process each frame, storing output.

  for (ff = 0; ff < nframes; ff++) {
    process_rasta_one_frame(alldata + ff*steppts, outdata+ff*mpf_private.runparam.nout,
			    &mpf_private.runparam, &mpf_private.history, mpf_private.window);
  }

  outbuf = mpf_voidstar_get_buffer(NULL, outdata);
  mpf_voidstar_set_length(outdata, sizeof(float)*nframes*mpf_private.runparam.nout);
  mpf_voidstar_push("output", outdata);
  
  // Save any needed in leftover.
  
  nleftover_new = nnewdata+mpf_private.nleftover - nframes*steppts;
  
  for (ii = 0; ii < nleftover_new; ii++) {
    mpf_private.leftover[ii] = alldata[ii+nframes*steppts];
  }
  mpf_private.nleftover = nleftover_new;
  free(alldata);

  return MPF_GOOD;
}



/* Component instance finalization. */
component_teardown() {
  if(mpf_private.runparam.history == TRUE) {
    save_history(&mpf_private.history, &mpf_private.runparam);
  }
  if (mpf_private.leftover) {
    free(mpf_private.leftover);
  }
  if (mpf_private.window) {
    free_fvec(mpf_private.window);
  }
}

