/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <grdf.h>

int main(int argc,char *argv[]) {
  GrdfWorld *world = NULL;
  GrdfOntology *ont;
  GrdfGraph *graph;
  GrdfNode *n1, *n2, *n3;
  GList *stmts;

  g_type_init();

  world = grdf_world_new();
  ont = grdf_ontology_new("http://www.w3.org/1999/02/22-rdf-syntax-ns#");
  grdf_world_add_ontology(world, ont, "rdf");

  graph = grdf_graph_new(world);
  grdf_stmt_new_auu(graph,"erik","isA","person");
  grdf_stmt_new_aul(graph,"erik","name","Erik Walthinsen");

  stmts = grdf_graph_search_anon(graph,"erik");
  grdf_stmt_fprintf(graph,GRDF_STMT(stmts->data),stdout);
  printf("\n");
}
