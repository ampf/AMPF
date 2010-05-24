//////////////////////////////////////////////////////////////////////
//
// Wrapper for C++ routines in gmm.cpp.
//
// Operates on the opaque data type cGMM.
//

extern "C" 
{
#include "gmmbridge.h"
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "gmm.h"

//////////////////////////////////////////////////////////////////////
//
// Local prototypes
//

// Fill in table values for fast log.
static void do_table(int n, float* lookup_table);

//////////////////////////////////////////////////////////////////////
//
// Globals
//

float *LOOKUP_TABLE;	// Used for fast lookup of exp.

//////////////////////////////////////////////////////////////////////
//
// Create a GMM from a .g,, file. The number of gaussians and the
// dimensionality had better match.
//
// Call gmm_free with the returned cGMM* when done.
//

cGMM* gmm_create(char* fname, int ngaussians, int dim) {
  FILE* fp;
  int nclusters;
  int cluster;
  GMM* gmm;
  
  gmm = new GMM();

  fp = fopen(fname, "r");
  if (!fp) {
    perror("Couldn't open gmm file");
    exit(EXIT_FAILURE);
  }
  gmm->Initialize(ngaussians, dim, 0);

  // Should always be one and only one cluster.
  nclusters = -1;
  cluster = -1;
  if (fscanf(fp, "Number of clusters: %d\n", &nclusters) != 1 || nclusters != 1) {
    fprintf(stderr, "Expected 1 cluster in gmm file %s\n", fname);
    exit(EXIT_FAILURE);
  }
  if (fscanf(fp, "Cluster %d\n", &cluster) != 1 || cluster != 0) {
    fprintf(stderr, "Expected 1 cluster in gmm file %s\n", fname);
    exit(EXIT_FAILURE);
  }
  
  gmm->ReadGMM(fp, dim);
  fclose(fp);
  return (cGMM*) gmm;
}

void gmm_print(cGMM* gmm_a) {
  GMM* gmm = (GMM*) gmm_a;
  gmm->PrintGMM(stdout);
}

void gmm_free(cGMM* gmm_a) {
  GMM* gmm = (GMM*) gmm_a;
  delete gmm;
}

float gmm_log_likelihood(cGMM* gmm_a, float* frame) {
  GMM* gmm = (GMM*) gmm_a;
  return gmm->Log_Likelihood(frame);
}

void initialize_table() {
  LOOKUP_TABLE = (float*) malloc(SIZETABLE*sizeof(float));
  if (!LOOKUP_TABLE) {
    perror("initialize_table failed");
    exit(EXIT_FAILURE);
  }
  do_table(N_LOOKUP_SIZE,LOOKUP_TABLE);
}

static void do_table(int n,float *lookup_table)
{
	float numlog;	
	int *const exp_ptr = ((int*)&numlog);
	int x = *exp_ptr;
	x = 0x00000000;
	x += 127 << 23;
        *exp_ptr = x;
	for(int i=0;i<pow(2,n);i++)
	{
		lookup_table[i]=log2(numlog);
		x+=1 << (23-n);
                *exp_ptr = x;				
	}
}

  
