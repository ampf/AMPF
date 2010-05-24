/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_H__
#define __GRDF_H__

#include <stdio.h>
#include <glib.h>
#include <glib-object.h>

G_BEGIN_DECLS

typedef GList GrdfStmtList;

/* these typedefs allow for the various objects to crossref each other */
typedef struct _GrdfWorld GrdfWorld;
typedef struct _GrdfGraph GrdfGraph;
typedef struct _GrdfOntology GrdfOntology;
typedef struct _GrdfStmt GrdfStmt;
typedef struct _GrdfNode GrdfNode;
typedef struct _GrdfNodeLiteral GrdfNodeLiteral;
typedef struct _GrdfNodeUri GrdfNodeUri;
typedef struct _GrdfNodeAnon GrdfNodeAnon;
typedef struct _GrdfNodeVariable GrdfNodeVariable;
//typedef struct _GrdfQuery GrdfQuery;

#include <grdf-world.h>
#include <grdf-ontology.h>
#include <grdf-graph.h>
#include <grdf-stmt.h>
#include <grdf-node.h>
#include <grdf-node-literal.h>
#include <grdf-node-uri.h>
#include <grdf-node-anon.h>
#include <grdf-node-variable.h>
// #include <grdf-template.h>
// #include <grdf-query.h>


void grdf_init();

G_END_DECLS

#endif /* __GRDF_H__ */
