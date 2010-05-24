/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <grdf.h>

enum {
  SIGNAL_0,
  /* FILL ME */
  LAST_SIGNAL
};

enum {
  ARG_0
  /* FILL ME */
};

static GObjectClass *parent_class = NULL;
//static guint grdf_query_signals[LAST_SIGNAL] = { 0 };

static void grdf_query_class_init (GrdfQueryClass * klass);
static void grdf_query_base_class_init (gpointer klass);
static void grdf_query_base_class_finalize (gpointer klass);

static void grdf_query_init (GrdfQuery *query);
static void grdf_query_finalize (GrdfQuery *query);

GType
grdf_query_get_type (void)
{
  static GType grdf_query_type = 0;

  if (G_UNLIKELY (grdf_query_type == 0)) {
    static const GTypeInfo grdf_query_info = {
      sizeof (GrdfQueryClass),
      grdf_query_base_class_init,
      grdf_query_base_class_finalize,
      (GClassInitFunc) grdf_query_class_init,
      NULL,
      NULL,
      sizeof (GrdfQuery),
      0,
      (GInstanceInitFunc) grdf_query_init,
      NULL
    };

    grdf_query_type = g_type_register_static (GRDF_TYPE_NODE, "GrdfQuery",
	&grdf_query_info, 0);
  }
  return grdf_query_type;
}

static void
grdf_query_class_init (GrdfQueryClass * klass)
{
  GObjectClass *gobject_class;
  GrdfNodeClass *grdf_node_class;

  gobject_class = (GObjectClass *) klass;
  grdf_node_class = (GrdfNodeClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize =
	(GObjectFinalizeFunc)grdf_query_finalize;
}

static void
grdf_query_base_class_init (gpointer klass)
{
//  GrdfQueryClass *grdf_query_class = GRDF_QUERY_CLASS (klass);
}

static void
grdf_query_base_class_finalize (gpointer klass)
{
//  GrdfQueryClass *grdf_query_class = GRDF_QUERY_CLASS (klass);
}

static void
grdf_query_init (GrdfQuery *query)
{
  query->stmts = NULL;
}

static void
grdf_query_finalize (GrdfQuery *node)
{
  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(node));
}

GrdfQuery *
grdf_query_new (GrdfWorld *world)
{
  GrdfQuery *query;

  g_assert(world != NULL);

  // Now allocate the anon object and fill it all in
  query = g_object_new (GRDF_TYPE_QUERY, NULL);
  g_assert(query != NULL);

  // Reference the world
  grdf_world_ref(world);
  query->world = world;

  return query;
}

GrdfQuery *
grdf_query_new_from_string (GrdfWorld *world, gchar *querystr)
{
  GrdfQuery *query = grdf_query_new(world);

  // FIXME!

  return query;
}

void
grdf_query_add_stmt (GrdfQuery *query, GrdfNode *subj, GrdfNode *pred, GrdfNode *obj)
{
  GrdfQueryStmt *stmt;

  stmt = g_new(GrdfQueryStmt,1);
  stmt->subj = subj;
  stmt->pred = pred;
  stmt->obj = obj;

  query->stmts = g_list_append(query->stmts, stmt);
}
