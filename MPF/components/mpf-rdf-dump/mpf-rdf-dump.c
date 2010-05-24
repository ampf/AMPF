/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-rdf.h>

#define COMPONENT_NAME "mpf-rdf-dump"
#define COMPONENT_DESC "Dump the RDF buffers to stdout"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_rdf_add_input("input");
}

component_init() {
}

component_process() {
  GrdfGraph *graph;
  int i;

  graph = mpf_rdf_pull("input");

  fprintf(stderr, "Graph at timestamp %llu\n",
      (long long unsigned int)mpf_buffer_get_timestamp(graph));
  for (i=0;i<grdf_graph_num_stmt(graph);i++) {
    grdf_stmt_fprintf(grdf_graph_get_stmt(graph,i),stderr);
  }

  mpf_rdf_unref(graph);

  return MPF_GOOD;
}
