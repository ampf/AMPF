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
//static guint grdf_stmt_signals[LAST_SIGNAL] = { 0 };

static void grdf_stmt_class_init (GrdfStmtClass *klass, gpointer class_data);
static void grdf_stmt_base_class_init (gpointer klass);
static void grdf_stmt_base_class_finalize (gpointer klass);

static void grdf_stmt_init (GrdfStmt *stmt);
static void grdf_stmt_finalize (GrdfStmt *stmt);

GType
grdf_stmt_get_type (void)
{
  static GType grdf_stmt_type = 0;

  if (G_UNLIKELY (grdf_stmt_type == 0)) {
    static const GTypeInfo grdf_stmt_info = {
      sizeof (GrdfStmtClass),
      grdf_stmt_base_class_init,
      grdf_stmt_base_class_finalize,
      (GClassInitFunc) grdf_stmt_class_init,
      NULL,
      NULL,
      sizeof (GrdfStmt),
      0,
      (GInstanceInitFunc) grdf_stmt_init,
      NULL
    };

    grdf_stmt_type = g_type_register_static (G_TYPE_OBJECT, "GrdfStmt",
	&grdf_stmt_info, 0);
  }
  return grdf_stmt_type;
}

static void
grdf_stmt_class_init (GrdfStmtClass *klass, gpointer class_data)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;

  gobject_class->finalize = 
	(GObjectFinalizeFunc) grdf_stmt_finalize;

  parent_class = g_type_class_peek_parent (klass);
}

static void
grdf_stmt_base_class_init (gpointer klass)
{
//  GrdfStmtClass *grdf_stmt_class = GRDF_STMT_CLASS (klass);
}

static void
grdf_stmt_base_class_finalize (gpointer klass)
{
//  GrdfStmtClass *grdf_stmt_class = GRDF_STMT_CLASS (klass);
}

static void
grdf_stmt_init (GrdfStmt *stmt)
{
  g_object_force_floating((GObject *)stmt);
}

static void
grdf_stmt_finalize (GrdfStmt *stmt)
{
  if (stmt->subj)
    grdf_node_unref(stmt->subj);
  if (stmt->pred)
    grdf_node_unref(stmt->pred);
  if (stmt->obj)
    grdf_node_unref(stmt->obj);
  if (stmt->subj)
    grdf_stmt_unref(stmt->ctx);

  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(stmt));
}


GrdfStmt *
grdf_stmt_new(GrdfGraph *graph, GrdfNode *subj, GrdfNode *pred, GrdfNode *obj)
{
  GrdfStmt *stmt;

  g_assert(GRDF_IS_NODE(subj));
  g_assert(GRDF_IS_NODE(pred));
  g_assert(GRDF_IS_NODE(obj));

  // Allocate a new statement object
  stmt = g_object_new (GRDF_TYPE_STMT, NULL);
  g_assert(stmt != NULL);

  // Fill in the nodes
  g_object_ref_sink(subj);
  stmt->subj = subj;
  g_object_ref_sink(pred);
  stmt->pred = pred;
  g_object_ref_sink(obj);
  stmt->obj = obj;

  // Add it to the graph if provided (should this be optional? prob not...)
  if (graph != NULL) {
    g_assert(GRDF_IS_GRAPH(graph));
    grdf_graph_add_stmt (graph, stmt);
  }

  return stmt;
}

GrdfStmt *
grdf_stmt_new_ctx(GrdfGraph *graph, GrdfNode *subj, GrdfNode *pred, GrdfNode *obj, GrdfStmt *ctx)
{
  GrdfStmt *stmt;

  g_assert(GRDF_IS_NODE(subj));
  g_assert(GRDF_IS_NODE(pred));
  g_assert(GRDF_IS_NODE(obj));
  g_assert(GRDF_IS_NODE(ctx));

  // Allocate a new statement object
  stmt = g_object_new (GRDF_TYPE_STMT, NULL);
  g_assert(stmt != NULL);

  // Fill in the nodes
  stmt->subj = subj;
  stmt->pred = pred;
  stmt->obj = obj;
  stmt->ctx = ctx;

  // Add it to the graph if provided (should this be optional? prob not...)
  if (graph != NULL) {
    g_assert(GRDF_IS_GRAPH(graph));
    grdf_graph_add_stmt (graph, stmt);
    g_object_unref(stmt);
  }

  return stmt;
}

void
grdf_stmt_fprintf(GrdfStmt *stmt, FILE *f)
{
  fprintf(f,"(");
  grdf_node_fprintf(stmt->subj,f);
  fprintf(f,", ");
  grdf_node_fprintf(stmt->pred,f);
  fprintf(f,", ");
  grdf_node_fprintf(stmt->obj,f);
  fprintf(f,")\n");
}

void
grdf_stmt_to_gstr_n3(GrdfStmt *stmt, GString *str)
{
  grdf_node_to_gstr_n3(stmt->subj,str);
  g_string_append_printf(str," ");
  grdf_node_to_gstr_n3(stmt->pred,str);
  g_string_append_printf(str," ");
  grdf_node_to_gstr_n3(stmt->obj,str);
  g_string_append_printf(str,".\n");
}
