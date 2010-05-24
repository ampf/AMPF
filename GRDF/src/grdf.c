/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <grdf.h>

void
grdf_init()
{
  g_type_init();
  // Register all types to avoid race conditions inside multi-threaded pipelines.
  GRDF_TYPE_GRAPH;
  GRDF_TYPE_NODE_ANON;
  GRDF_TYPE_NODE_LITERAL;
  GRDF_TYPE_NODE_URI;
  GRDF_TYPE_NODE;
  GRDF_TYPE_ONTOLOGY;
  GRDF_TYPE_STMT;
  GRDF_TYPE_WORLD;
  //  GRDF_TYPE_QUERY;
  //  GRDF_TYPE_TEMPLATE;
}
