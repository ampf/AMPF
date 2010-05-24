/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <memory.h>

//#define MIN_DURATION 200 // number of frames
#define LOG_0_5 -0.693147181
#define LOG_0_9 -0.105360516
#define LOG_0_1 -2.302585093
#define N_LOOPS_ADAPT 5 //5
#define N_LOOPS_ADAPT2 3 //3
#define N_LOOPS_ADAPT3 5
//#define N_LOOPS_MERGE 2
#define N_THRES_CHANGE 0
#define N_THRES_CHANGE_PERCENT 0
//#define MINVALUEFORMINUSLOG -103.0 // -18.42
#define MINVALUEFORMINUSLOG -1000.0 // -18.42
#define MIN_POS_FLOAT 1.4e-45
#define N_LOOPS_K_MEANS 15
#define LAST 0
#define CURR 1
#define P 2	//variable to decide the initialization
#define MIN_FLOAT -3.40282346638528860e+38
#define MERGE_IN_ONE_GO 10
#define N_LOOKUP_SIZE 12
#define SIZETABLE 4096
#define FULL_HEADER 1
#define MIN_VARIANCE 0.01

#define MIN_MERGE_SCORE 2000 //threshold to merge neighbours
//! \addtogroup gmm
//! \{
//!
extern float *LOOKUP_TABLE;
void Initialize1DFloat(float *array, int dim);
void Initialize1DInt(int *array, int dim);
void Initialize2DFloat(float **array, int dim1, int dim2);
void Initialize1DDouble(double *array, int dim);
void Initialize2DDouble(double **array, int dim1, int dim2);
//! \}
//!
#endif
