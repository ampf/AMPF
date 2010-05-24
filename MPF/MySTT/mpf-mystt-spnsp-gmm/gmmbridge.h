/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef INCLUDE_GMMBRIDGE_H
#define INCLUDE_GMMBRIDGE_H

//////////////////////////////////////////////////////////////////////
//
// Types
//

//
// Used as an opaque bridge for the C++ class GMM
//

typedef void* cGMM;

//////////////////////////////////////////////////////////////////////
//
// Prototypes
//

cGMM* gmm_create(char* fname, int ngaussians, int dim);
void gmm_print(cGMM*);	// For debugging
void gmm_free(cGMM*);
float gmm_log_likelihood(cGMM*, float*);

void initialize_table();	// Used for fast log.

#endif
