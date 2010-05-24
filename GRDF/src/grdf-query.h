/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_QUERY_H__
#define __GRDF_QUERY_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_QUERY (grdf_query_get_type())
#define GRDF_IS_QUERY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_QUERY))
#define GRDF_IS_QUERY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_QUERY))
#define GRDF_QUERY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_QUERY, GrdfQueryClass))
#define GRDF_QUERY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_QUERY, GrdfQuery))
#define GRDF_QUERY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_QUERY, GrdfQueryClass))
#define GRDF_QUERY_CAST(obj) ((GrdfQuery*)(obj))

//typedef struct _GrdfQuery GrdfQuery;
typedef struct _GrdfQueryClass GrdfQueryClass;

typedef struct _GrdfQueryStmt GrdfQueryStmt;
typedef struct _GrdfQueryResults GrdfQueryResults;

struct _GrdfQuery
{
  GObject object;

  GrdfWorld *world;

  GList *stmts;
};

struct _GrdfQueryClass
{
  GObjectClass parent_class;
};

struct _GrdfQueryStmt
{
  GrdfNode *subj;
  GrdfNode *pred;
  GrdfNode *obj;
};

struct _GrdfQueryResults
{
  GList *variables;
  GArray *rows;
};

GType grdf_query_get_type (void);

GrdfQuery *grdf_query_new (GrdfWorld *world);
GrdfQuery *grdf_query_new_from_string (GrdfWorld *world, gchar *querystr);

void grdf_query_add_stmt (GrdfQuery *query, GrdfNode *subj, GrdfNode *pred, GrdfNode *obj);


G_END_DECLS

#endif /* __GRDF_QUERY_H__ */
