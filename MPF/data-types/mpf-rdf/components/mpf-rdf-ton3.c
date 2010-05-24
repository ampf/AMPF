/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-rdf.h>
#include <mpf/mpf-voidstar.h>

#define COMPONENT_NAME "mpf-rdf-ton3"
#define COMPONENT_DESC "RDF to N3 text"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpf-componenttemplate.h>

component_class_init() {
  mpf_rdf_add_input("input");
  mpf_voidstar_add_output_with_caps("output", "text/plain");
}

component_init() {
}

component_process() {
  GrdfGraph *graph = mpf_rdf_pull("input");
  gchar *n3 = NULL;
  gint len = 0;

  // Have libgrdf write the graph to a string it will allocate
  grdf_graph_to_n3(graph,&n3,&len);

  // Push the buffer downstream with the appropriate length
  mpf_voidstar_push_with_len("output",n3,len-1);

  return MPF_GOOD;
}

