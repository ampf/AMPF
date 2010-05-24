
//////////////////////////////////////////////////////////////////////
//
// This is a probably non-functional version of rasta_mpf designed for
// debugging from the command line. It does not use MPF or gstreamer,
// but rather just reads from stdin.
//
// Adam Janin <janin@icsi.berkeley.edu>
//

/************************************************************************
 *                                                                       *
 *               ROUTINES IN THIS FILE:                                  *
 *                                                                       *
 *                      main(): main calling routine                     *
 *                                                                       *
 ************************************************************************/

/***********************************************************************

 (C) 1995 US West and International Computer Science Institute
     2009 International Computer Science Institute, Appscio
     All rights reserved
 U.S. Patent Numbers 5,450,522 and 5,537,647

 Embodiments of this technology are covered by U.S. Patent
 Nos. 5,450,522 and 5,537,647.  A limited license for internal
 research and development use only is hereby granted.  All other
 rights, including all commercial exploitation, are retained unless a
 license has been granted by either US West or International Computer
 Science Institute.

***********************************************************************/

/********************************************************************
	Scope of Algorithm


The actual processing is split up into a few pieces:

        1) power spectral analysis
        2) auditory spectrum computation
        3) compression (possibly adaptive)
        4) temporal filtering of each channel trajectory
        5) expansion (also possibly adaptive)
        6) postprocessing (e.g., preemphasis, loudness compression)
        7) autoregressive all-pole modeling (cepstral coefficients)

and i/o can either be ascii, binary (shorts on the input and floats
on the output), or standard ESPS files.

Since the lowest frequency and highest frequency bands extend
into forbidden territory (negative or greater-than-Nyquist
frequencies), they are ignored for most of the analysis. This
is done by computing a variable called first_good (which for
1 bark spacing is 1) and generally ignoring the first and last
``first_good'' channels. Just prior to the all-pole modeling,
the values from the good channels are copied over to the questionable
ones. (Note that ``first_good'' is available to most routines
via a pointer to a general parameter structure that has such
useful things as the number of filters, the analysis stepsize
in msec, the sampling rate, etc. . See rasta.h for
a definition of this structure).


This program (Rasta 2.0) implements the May 1994 version of RASTA-PLP
analysis. It incorporates several primary pieces:

1) PLP Analysis - as described in Hermansky's 1990 JASA paper,
the basic form of spectral analysis is Perceptual Linear Prediction,
or PLP. This computes the cepstral parameters of an all-pole model
of an auditory spectrum, which is a power spectrum that
has been frequency warped to the bark scale, smoothed by
an asymmetric critical-band trapezoid function,
down-sampled into approximately 1 Bark intervals,
cube root compressed for an intensity-loudness transformation, 
and weighted by a fixed equal loudness curve. 

2) RASTA filtering - as described in several Hermansky and Morgan
papers, the basic idea here is to bandpass filter the trajectories
of the spectral parameters. In the case of RASTA-PLP, this filtering
is done on a nonlinear transformation of an auditory-like spectrum, 
prior to the autoregressive modeling in PLP.

3) J-processing - For RASTA, the bandpass filtering is done in the 
log domain. An alternative is to use the J-family
of log-like curves

	y = log(1 + Jx)

where J is a constant that can appears to be optimally set when
it is inversely proportional to the noise power, (currently typically
1/3 of the inverse noise), x is the
critical band value, and y is the non-linearly transformed critical 
band value.
Rather than do the true inverse, which would be

	x = (exp(y) - 1)/J

and could get negative values, we use

	x' = (exp(y))/J

This prevents negative values, and in doing so effectively adds noise
floor by adding 1/J to the true inverse. 

One way of doing J-processing is to pick one constant J value and enter
this value at the command line. This J value should be dependent on the
SNR of the speech. We also may want to estimate the noise level for 
adaptive settings of the J-parameter during the utterance. 
Both methods of picking J should be handled with care. For the first
case, see the README file for a discussion of the perils of using even
a default J if it is too far from what you really need;
if the application situation is relatively fixed, you are better off making
an off-line noise measurement to get a good J value; in any event some
experimentation will soon show the proper constants involved for a problem.
For the second case, noise level is estimated for adaptive settings of the
J-parameter during the utterance. This should be done with care as well, 
as the use of a time-varying J brings in a new complication that you must
consider in the training and recognition, since changing J's over a time
series introduces a new source of variability in the analysis that must
be accounted for. The different J values, as required by differing noise
conditions, generates different spectral shapes and dynamics of the spectra.
This means that the training system must contend with a new source of 
variability due to the change in processing strategy that is adaptively
determined from the data. One approach to handle this variability is by
doing spectral mapping. In the current version, Spectral mapping is 
performed whenever J-Rasta processing is used with adaptive Js.

Spectral mapping - transform the spectrum processed with a J-value  
                   corresponding to noisy speech to a spectrum processed
                   with a J value for clean speech. In other words, we
                   find a mapping between log(1+xJ) and log(1+xJref) 
                   where Jref is the reference J, i.e. J value for clean speech.
                   For this approach, we have used a multiple regression 
                   within each critical band. In principle, this solution
                   reduces the variability due to the choice of J, and so
                   minimizes the effect on the training process.
       

How this works is:

1) Training of the recognizer:
   -- Train the recognizer with clean speech processed with J = 1.0e-6, a 
      suitable J value for clean speech.

2) Finding the relationship between spectrum corresponding to different Jah 
   values to the spectrum corresponding to J = 1.0e-6
   -- For each of the Jahs in the set {3.16e-7, 1.0e-7, 3.16e-8, 1.0e-8, 
      3.16e-9, 1.0e-9}, find a mapping relationship of the corresponding 
      bandpass filtered spectrum to the spectrum corresponding to J =
      1.0e-6. In other words, find a set of mapping coefficients for each
      Jah to 1.0e-6. The mapping method will be discussed later.

3) Extracting the speech features for the testing speech data
   -- obtain the critical band values as in PLP
   -- estimate the noise energy and thus the Jah value. Call this Jah value 
      J(orig).
   -- Pick a Jah from the set {3.16e-7, 1.0e-7, 3.16e-8, 1.0e-8,3.16e-9, 1.0e-9}
      that is closest to J(orig) and call this J(quant).
   -- perform the non-linear transformation of the spectrum using 
      log (1+J(quant)* X).
   -- bandpass filter the transformed critical band values.
   -- use the set of mapping coefficients for J(quant) to do the spectral 
      mapping or spectral transformation.
   -- preemphasize via the equal-loudness curve and then perform amplitude
      compression using the intensity-loudness power law as in PLP
   -- take the inverse of the non-linear function. 
   -- compute the cepstral parameters for the AR model.   
    
    
 
How regression coefficients are computed in our experiment:

    In order to map critical band values(after bandpass filtering) processed
    with different J values to those processed with J = 1.0e-6, J-Rasta 
    filtered critical band outputs from 10 speakers(excluded from the training
    and testing sets) are used to train 
    multiple regression models.  
    For example, for mapping from J= J(quant) to J = 1.0e-6, the regression
    equation can be written as:

    Yi = B2i* X2 + B3i* X3 + ... + B16i * X16 + B17i       (**)

where Yi = i th bandpass filtered critical band processed with J=1.0e-6
           i = 2, .. 16
      X2, X3, ... X16    2rd to 16th bandpass filtered critical band values
                         processed with J = J(quant), where
                         J(quant) is in the set 
                         {3.16e-7, 1.0e-7, 3.16e-8, 1.0e-8,3.16e-9, 1.0e-9} 
      B2i, B3i ... B17i     are the 16  mapping coefficients 

      
    For equation (**), we have made the assumption that the sampling frequency
    is 8kHz and the number of critical bands is seventeen. The first and 
    the last bands extend into forbidden territory -- negative or greater
    than Nyquist frequencies. Thus the the two bands are ignored for most
    of the analysis. Their values are made equal to the adjacent band's just
    before the autoregressive all-pole modeling. This is why we only make
    Yi dependent on bandpass filtered critical bands X2,X3,... X16, altogether
    fifteen critical bands. 

    The default mapping coefficients sets is stored in map_weights.dat. 
    This is suitable for s.f. 8kHz, 17 critical bands. For users who have 
    a different setup, they may want to find their own mapping coefficients
    set. This could be done by using the command options -R and -f. Command 
    -R allows you to get bandpass filtered critical band values as output
    instead of cepstral coefficients. 
    These outputs could be used as regression data. A simple multiple
    regression routine can be used to generate the mapping coefficients
    from these regression data. These mapping coefficients can be stored
    in a file. Command -f allows you to use this file to replace the
    default file map_weights.dat. The format of this file is:

     beginning of file 
    <Total number of Jahs, for the example shown above, it is [7] > 
    <# of critical bands, for the setup for 8kHz, this is [15]>
    <# of mapping coefficients/band, for the setup for 8kHz, this is [16]>
    
    <The J for clean speech, [1.0e-6]>
   
    <mapping coefficients for Y2, [B22, B32, B42,...]>
    <mapping coefficients for Y3, [B23, B33, B43 ...]>
        |
        |
        |
        V
    <mapping coefficients for Y16>
 
    
    <The second largest Jah besides 1e-6, [3.16e-7]>
        
        |
        |      mapping coefficients
        |
        |
        V 
  
    <The third largest Jah besides 1e-6, [1.0e-7]>
       
        |
        |      mapping coefficients
        |
        |
        V 
  
     .
     .
     .
     .    
     end of file



*********************************************************************/
#include <stdio.h>
#include <math.h>
#include "rasta.h"
#include "config.h"


//////////////////////////////////////////////////////////////////////
//
// Prototypes
//

int init_rasta();
void process_rasta(short*, int);
void process_rasta_one_frame(float*);

static void reset_state(struct param *runparam, struct fhistory *history);

/******************************************************/

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

// For debugging
struct component_private {
  struct param runparam;
  struct fhistory history;

  // Data left over from previous reads. At most window size floats.
  float* leftover;
  int nleftover;	// Number of actual valid points in leftover.

  struct fvec* window;	// Hamming window parameters
  
  // For debugging
  FILE *outfp;
} mpf_private;

// For debugging. Read some number of shorts from stdin and pass
// them to the processing routines.

int main(int argc, char** argv) {
  int nread;
  int npts;
  short* inbuf;
  
  init_rasta();

  // main analysis loop. Read from stdin and provide data to
  // processing routines.

  // For testing, npts should be set to random values...
  npts = 65500;
  inbuf = (short*) malloc(sizeof(short)*npts);
  if (!inbuf) {
    perror("malloc failed in rasta");
    exit(-1);
  }
  
  do {
    nread = fread(inbuf, sizeof(short), npts, stdin);
    process_rasta(inbuf, nread);
  } while (nread == npts);

  delete_rasta();

  free(inbuf);
  
  return 1;
}

int
init_rasta()
{
  
  char *argv[] = { 
    "rasta_mpf",
    "-O",	// Online. Required.
    "-M",	// dither. Optional.
//    "-F",	// high pass filter. Optional.
//   "-T",	// Swap input bytes endianess. Optional.
     "-U",	// Swap output byte endianess. Probably required.
    "-w 25",	// Window size in ms
    "-s 10",	// Step size in ms
    "-S 16000",	// Sampling frequency
    "-m 18",	// Model order
    "-A",	// Ascii output for testing. ???
  };
  
  int argc = sizeof(argv)/sizeof(char*);

  get_comline(&mpf_private.runparam, argc,argv); /* preferably list arguments in 
					command line so there is a record */

  check_args( &mpf_private.runparam ); /* Exits if params out of range */

  /* If the debugging is on, show the command-line argurments */

  if (mpf_private.runparam.debug == TRUE) {
      show_args( &mpf_private.runparam );
  }

  if (mpf_private.runparam.infname == NULL) {
      mpf_private.runparam.infname = "-";
  }
  if (mpf_private.runparam.outfname == NULL) {
      mpf_private.runparam.outfname = "-";
  }

  // FOR DEBUGGING. Write to a file.   ???
  mpf_private.runparam.outfname = "rastatestout.asc";

  mpf_private.history.eof = FALSE;

  mpf_private.outfp = open_out( &mpf_private.runparam ); /* Open output file */

  init_param(0, &mpf_private.runparam); /* Compute necessary parameters 
					   for analysis */
  
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
  
  return 0;
}

int delete_rasta() {
  if(mpf_private.runparam.history == TRUE) {
    save_history(&mpf_private.history, &mpf_private.runparam);
  }
  if (mpf_private.leftover) {
    free(mpf_private.leftover);
  }
  if (mpf_private.window) {
    free_fvec(mpf_private.window);
  }
        
  // Debugging.
  if (mpf_private.outfp != stdout) {
    fclose(mpf_private.outfp);
  }


}

// Ugly historic endian issues.
//
// Always reads and writes big-endian unless overridden by command line
// flags.  This should probably be changed to always read and write
// native endianness, and have endian switchers in the gstream as
// needed.

static void endian_swap_short(short *sbuf, int nshorts) {
    /* Swap the bytes in a buffer full of shorts if this machine 
       is not big-endian */
    int i, x;
    for (i=0; i<nshorts; ++i) {
	x = *sbuf;
	*sbuf++ = ((x & 0xFF) << 8) + ((x >> 8) & 0xFF);
    }
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


void process_rasta(short* newdata, int nnewdata) {
  float* alldata;
  int ii, ff;
  int nframes;
  int winpts;
  int steppts;
  int nleftover_new;

  //  fprintf(stderr, "\nnleftover %d\nnnewdata %d\n", mpf_private.nleftover, nnewdata);

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
    return;
  }

  // If we get here, we have enough for at least one frame.

  nframes = 1 + (int) ((nnewdata+mpf_private.nleftover-winpts)/steppts);


  //  fprintf(stderr, "nframes %d\n", nframes);

  // Create a buffer for the left over data plus the new data. Inefficient.

  alldata = (float*) malloc(sizeof(float)*(nnewdata+mpf_private.nleftover));
  if (!alldata) {
    perror("malloc for data failed in rasta");
    exit(-1);
  }

  // Debugging. Fill with -123.456

  //  for (ii = 0; ii < nnewdata+mpf_private.nleftover; ii++) {
  // alldata[ii] = -123.456;
  //  }

    
  for (ii = 0; ii < mpf_private.nleftover; ii++) {
    alldata[ii] = mpf_private.leftover[ii];
    //    fprintf(stderr, "alldata[%-3d] = %6.2f\n", ii, alldata[ii]);
  }
  for (; ii < nnewdata+mpf_private.nleftover; ii++) {
    alldata[ii] = (float) newdata[ii-mpf_private.nleftover];
    // fprintf(stderr, "alldata[%-3d] = %6.2f\n", ii, alldata[ii]);
  }

  for (ff = 0; ff < nframes; ff++) {
    //    fprintf(stderr, "\ncalling process_rasta_one_frame on %d\n\n", alldata+ff*steppts);
    process_rasta_one_frame(alldata + ff*steppts);
  }

  // Save any needed in leftover.
 
  nleftover_new = nnewdata+mpf_private.nleftover - nframes*steppts;

  //  fprintf(stderr, "nleftover_new %d\n", nleftover_new);
  
  for (ii = 0; ii < nleftover_new; ii++) {
    mpf_private.leftover[ii] = alldata[ii+nframes*steppts];
  }
  mpf_private.nleftover = nleftover_new;
  free(alldata);
}

void process_rasta_one_frame(float* indata) {
  int ii;
  static struct fvec* frame = NULL;
  struct fvec* cepstrum;

  if (frame == NULL) {
    frame = alloc_fvec(mpf_private.runparam.winpts);
  }  

  // Don't clobber the indata
  for (ii = 0; ii < frame->length; ii++) {
    frame->values[ii] = indata[ii];
    // Debugging. Check valid
    // if (fabs(indata[ii]+123.456) < 0.001) {
    //      fprintf(stderr, "Bad value at %d\n", ii);
    //}
    //    fprintf(stderr, "%-3d %6.2f\n", ii, frame->values[ii]);
  }
  //  fprintf(stderr, "\n\n");
    
  // High pass filter if requested.
if (mpf_private.runparam.HPfilter == TRUE) {
    fvec_HPfilter(&mpf_private.history, &mpf_private.runparam, frame);
  }

  // Apply hamming window.
  for (ii = 0; ii < mpf_private.runparam.winpts; ii++) {
    frame->values[ii] = frame->values[ii] * mpf_private.window->values[ii];
  }

  // Compute the features.
  cepstrum = rastaplp( &mpf_private.history, &mpf_private.runparam, frame );

  // Output
  if(cepstrum->length) {
    write_out( &mpf_private.runparam, mpf_private.outfp, cepstrum );
  }
}


