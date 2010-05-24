/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
//
// $Id: gmm.h,v 1.1.1.1 2005/05/06 01:15:31 xanguera Exp $
//
#ifndef __GMM_H__
#define __GMM_H__

#include "global.h"

class Gaussian {
 public:
  int DIM;
  float *Mean;
  float CP;
  float *Var;
  Gaussian();
  ~Gaussian();
  void Initialize(int dim);
  void calculateCP(void);
  inline float Log_Likelihood(float *feature);
  void PrintGaussian(FILE *fout);
  void ReadGaussian(FILE *fin, const int d);
};

class GMM {
 public:
  int M;
  int DIM;
  int NSAMPLES;
  float *Weight;
  Gaussian *gArray;	
  float *local_lkld;
  float lkld;
  GMM();
  GMM(const GMM &gmm);
  ~GMM();
  void Copy(GMM temp_gmm);
  float Train(float *data, int nSamples, double *times);
  float Train_EM(float *data, int nSamples, double *times,int doeval);
  float Evaluate(float *data, int nSamples);
	float MAP_adaptation(float *data, int nSamples, double *times,float rel_factor);
	float MAP_means_adaptation(float *data, int nSamples, double *times,float rel_factor);
  void Initialize(int M, int dim, int nSamples);
  void Initialize(float *data, int count);
  float Log_Likelihood(float *feature);
  float Log_Likelihood_KL(float *feature);
  inline float Log_Add(float log_a, float log_b);
  void PrintGMM(FILE *fout);
  void ReadGMM(FILE *fin, const int d);
};
inline float lut_log (float val, float *lookup_table, int n);
double currenttime();
#endif

