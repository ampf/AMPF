/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <grdf.h>
#include <string.h>

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
//static guint grdf_graph_signals[LAST_SIGNAL] = { 0 };

static void grdf_graph_class_init (GrdfGraphClass *klass, gpointer class_data);
static void grdf_graph_base_class_init (gpointer klass);
static void grdf_graph_base_class_finalize (gpointer klass);
static void grdf_graph_init (GrdfGraph * grdf_graph);

static void grdf_graph_finalize (GrdfGraph *graph);


GType
grdf_graph_get_type (void)
{
  static GType grdf_graph_type = 0;

  if (G_UNLIKELY (grdf_graph_type == 0)) {
    static const GTypeInfo grdf_graph_info = {
      sizeof (GrdfGraphClass),
      grdf_graph_base_class_init,
      grdf_graph_base_class_finalize,
      (GClassInitFunc) grdf_graph_class_init,
      NULL,
      NULL,
      sizeof (GrdfGraph),
      0,
      (GInstanceInitFunc) grdf_graph_init,
      NULL
    };

    grdf_graph_type = g_type_register_static (G_TYPE_OBJECT, "GrdfGraph",
  &grdf_graph_info, 0);
  }
  return grdf_graph_type;
}

static void
grdf_graph_class_init (GrdfGraphClass *klass, gpointer class_data)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_peek_parent(klass);

  gobject_class->finalize =
    (GObjectFinalizeFunc) grdf_graph_finalize;
}

static void
grdf_graph_base_class_init (gpointer klass)
{
//  GrdfGraphClass *grdf_graph_class = GRDF_GRAPH_CLASS (klass);
}

static void
grdf_graph_base_class_finalize (gpointer klass)
{
//  GrdfGraphClass *grdf_graph_class = GRDF_GRAPH_CLASS (klass);
}

static void
grdf_graph_init (GrdfGraph *graph)
{
  graph->stmts = NULL;
  graph->numstmts = 0;
}

static void
grdf_graph_finalize (GrdfGraph *graph)
{
  GrdfStmtList *stmts;
  GrdfStmt *stmt;

  /* Unreference all the statements so onesies can be freed */
  stmts = graph->stmts;
  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);
    grdf_stmt_unref(stmt);

    stmts = g_list_next(stmts);
  }

  /* Unref the world structure as well */
  grdf_world_unref(graph->world);

  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(graph));
}

GrdfGraph *
grdf_graph_make_writable(GrdfGraph *graph)
{
  if (((GObject *)graph)->ref_count >= 1) {
    return grdf_graph_copy(graph);
  } else {
    return graph;
  }
}

GrdfGraph *
grdf_graph_copy(GrdfGraph *graph)
{
  GrdfGraph *newgraph;

  newgraph = grdf_graph_new(graph->world);

  // FIXME

  return newgraph;
}

GrdfGraph *
grdf_graph_new (GrdfWorld *world)
{
  GrdfGraph *graph;

  g_assert(world != NULL);
  g_assert(GRDF_IS_WORLD(world));

  graph = g_object_new (GRDF_TYPE_GRAPH, NULL);

  g_object_ref(world);
  graph->world = world;

  return graph;
}

void
grdf_graph_add_stmt (GrdfGraph *graph, GrdfStmt *stmt)
{
  g_assert(GRDF_IS_GRAPH(graph));
  g_assert(GRDF_IS_STMT(stmt));

  // Add a reference to the statement
  g_object_ref_sink(stmt);

  // Add the stmt to the master list
  graph->stmts = g_list_prepend(graph->stmts, stmt);
  graph->numstmts++;
}


gint
grdf_graph_num_stmt(GrdfGraph *graph)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));

  return graph->numstmts;
}

GrdfStmt *
grdf_graph_get_stmt(GrdfGraph *graph, gint index)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));

  g_assert(index < graph->numstmts);

  return (GrdfStmt *)g_list_nth_data(graph->stmts, index);
}

void
grdf_graph_fprintf(FILE *f,GrdfGraph *graph)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));

  fprintf(f,"Graph has %d nodes:\n",graph->numstmts);
}

void grdf_graph_dump(FILE *f, GrdfGraph *graph) {
    gchar *n3 = NULL;
    gint len = 0;

    // Have libgrdf write the graph to a string it will allocate
    grdf_graph_to_n3(graph,&n3,&len);
    fprintf(f, n3);
}

/********** Search primitives, find *all* matching **********/
GList *
grdf_stmtlist_find_subj_node(GrdfStmtList *stmts, GrdfNode *subj)
{
  GList *stmtlist = NULL;
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(subj != NULL);
  g_assert(GRDF_IS_NODE(subj));

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (grdf_node_equal(stmt->subj,subj))
      stmtlist = g_list_prepend(stmtlist, stmt);

    stmts = g_list_next(stmts);
  }

  return stmtlist;
}

GList *
grdf_stmtlist_find_subj_uri(GrdfStmtList *stmts, const gchar *subj)
{
  GList *stmtlist = NULL;
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(subj != NULL);

  while (stmts != NULL) {
    stmt = GRDF_STMT(stmts->data);

    if (GRDF_IS_NODE_URI(stmt->subj) && grdf_node_uri_match(stmt->subj, subj))
      stmtlist = g_list_prepend(stmtlist, stmt);

    stmts = g_list_next(stmts);
  }

  return stmtlist;
}

GList *
grdf_stmtlist_find_pred_node(GrdfStmtList *stmts, GrdfNode *pred)
{
  GList *stmtlist = NULL;
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(pred != NULL);
  g_assert(GRDF_IS_NODE(pred));

  while (stmts != NULL) {
    stmt = GRDF_STMT(stmts->data);

    if (grdf_node_equal(stmt->pred,pred))
      stmtlist = g_list_prepend(stmtlist, stmt);

    stmts = g_list_next(stmts);
  }

  return stmtlist;
}

GList *
grdf_stmtlist_find_pred_uri(GrdfStmtList *stmts, const gchar *pred)
{
  GList *stmtlist = NULL;
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(pred != NULL);

  while (stmts != NULL) {
    stmt = GRDF_STMT(stmts->data);

    if (GRDF_IS_NODE_URI(stmt->pred) && grdf_node_uri_match(stmt->pred, pred))
      stmtlist = g_list_prepend(stmtlist, stmt);

    stmts = g_list_next(stmts);
  }

  return stmtlist;
}

GList *
grdf_stmtlist_find_obj_node(GrdfStmtList *stmts, GrdfNode *obj)
{
  GList *stmtlist = NULL;
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(obj != NULL);
  g_assert(GRDF_IS_NODE(obj));

  while (stmts != NULL) {
    stmt = GRDF_STMT(stmts->data);

    if (grdf_node_equal(stmt->obj,obj))
      stmtlist = g_list_prepend(stmtlist, stmt);

    stmts = g_list_next(stmts);
  }

  return stmtlist;
}

GList *
grdf_stmtlist_find_obj_uri(GrdfStmtList *stmts, const gchar *obj)
{
  GList *stmtlist = NULL;
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(obj != NULL);

  while (stmts != NULL) {
    stmt = GRDF_STMT(stmts->data);

    if (GRDF_IS_NODE_URI(stmt->obj) && grdf_node_uri_match(stmt->obj,obj))
      stmtlist = g_list_prepend(stmtlist, stmt);

    stmts = g_list_next(stmts);
  }

  return stmtlist;
}

GList *
grdf_stmtlist_find_ctx(GrdfStmtList *stmts, GrdfStmt *ctx)
{
  GList *stmtlist = NULL;
  GrdfStmt *stmt;

  while (stmts != NULL) {
    stmt = GRDF_STMT(stmts->data);

    if (stmt->ctx == ctx)
      stmtlist = g_list_prepend(stmtlist, stmt);

    stmts = g_list_next(stmts);
  }

  return stmtlist;
}


/**********  **********/
GList *
grdf_graph_find_subj_node(GrdfGraph *graph, GrdfNode *subj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_subj_node(graph->stmts,subj);
}

GList *
grdf_graph_find_subj_uri(GrdfGraph *graph, const gchar *subj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_subj_uri(graph->stmts,subj);
}

GList *
grdf_graph_find_pred_node(GrdfGraph *graph, GrdfNode *pred)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_pred_node(graph->stmts,pred);
}

GList *
grdf_graph_find_pred_uri(GrdfGraph *graph, const gchar *pred)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_pred_uri(graph->stmts,pred);
}

GList *
grdf_graph_find_obj_node(GrdfGraph *graph, GrdfNode *obj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_obj_node(graph->stmts,obj);
}

GList *
grdf_graph_find_obj_uri(GrdfGraph *graph, const gchar *obj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_obj_uri(graph->stmts,obj);
}

GList *
grdf_graph_find_ctx(GrdfGraph *graph, GrdfStmt *ctx)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_ctx(graph->stmts,ctx);
}




/********** Search primitives, find *first* matching **********/
GrdfStmt *
grdf_stmtlist_find_first_subj_node(GrdfStmtList *stmts, GrdfNode *subj)
{
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(subj != NULL);
  g_assert(GRDF_IS_NODE(subj));

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (grdf_node_equal(stmt->subj,subj))
      return stmt;

    stmts = g_list_next(stmts);
  }

  return NULL;
}

GrdfStmt *
grdf_stmtlist_find_first_subj_uri(GrdfStmtList *stmts, const gchar *subj)
{
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(subj != NULL);

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (GRDF_IS_NODE_URI(stmt->subj) && grdf_node_uri_match(stmt->subj, subj))
      return stmt;

    stmts = g_list_next(stmts);
  }

  return NULL;
}

GrdfStmt *
grdf_stmtlist_find_first_pred_node(GrdfStmtList *stmts, GrdfNode *pred)
{
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(pred != NULL);
  g_assert(GRDF_IS_NODE(pred));

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (grdf_node_equal(stmt->pred,pred))
      return stmt;

    stmts = g_list_next(stmts);
  }

  return NULL;
}

GrdfStmt *
grdf_stmtlist_find_first_pred_uri(GrdfStmtList *stmts, const gchar *pred)
{
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(pred != NULL);

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (GRDF_IS_NODE_URI(stmt->pred) && grdf_node_uri_match(stmt->pred, pred))
      return stmt;

    stmts = g_list_next(stmts);
  }

  return NULL;
}

GrdfStmt *
grdf_stmtlist_find_first_obj_node(GrdfStmtList *stmts, GrdfNode *obj)
{
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(obj != NULL);
  g_assert(GRDF_IS_NODE(obj));

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (grdf_node_equal(stmt->obj,obj))
      return stmt;

    stmts = g_list_next(stmts);
  }

  return NULL;
}

GrdfStmt *
grdf_stmtlist_find_first_obj_uri(GrdfStmtList *stmts, const gchar *obj)
{
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(obj != NULL);

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (GRDF_IS_NODE_URI(stmt->obj) && grdf_node_uri_match(stmt->obj,obj))
      return stmt;

    stmts = g_list_next(stmts);
  }

  return NULL;
}

GrdfStmt *
grdf_stmtlist_find_first_ctx(GrdfStmtList *stmts, GrdfStmt *ctx)
{
  GrdfStmt *stmt;

  g_assert(stmts != NULL);
  g_assert(ctx != NULL);
  g_assert(GRDF_IS_STMT(ctx));

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    if (stmt->ctx == ctx)
      return stmt;

    stmts = g_list_next(stmts);
  }

  return NULL;
}


GrdfStmt *
grdf_graph_find_first_subj_node(GrdfGraph *graph, GrdfNode *subj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_first_subj_node(graph->stmts,subj);
}

GrdfStmt *
grdf_graph_find_first_subj_uri(GrdfGraph *graph, const gchar *subj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_first_subj_uri(graph->stmts,subj);
}

GrdfStmt *
grdf_graph_find_first_pred_node(GrdfGraph *graph, GrdfNode *pred)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_first_pred_node(graph->stmts,pred);
}

GrdfStmt *
grdf_graph_find_first_pred_uri(GrdfGraph *graph, const gchar *pred)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_first_pred_uri(graph->stmts,pred);
}

GrdfStmt *
grdf_graph_find_first_obj_node(GrdfGraph *graph, GrdfNode *obj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_first_obj_node(graph->stmts,obj);
}

GrdfStmt *
grdf_graph_find_first_obj_uri(GrdfGraph *graph, const gchar *obj)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_first_obj_uri(graph->stmts,obj);
}

GrdfStmt *
grdf_graph_find_first_ctx(GrdfGraph *graph, GrdfStmt *ctx)
{
  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  return grdf_stmtlist_find_first_ctx(graph->stmts,ctx);
}



void
grdf_graph_to_n3(GrdfGraph *graph,gchar **str,gint *len)
{
  GString *acc;
  GList *stmts;
  GrdfStmt *stmt;

  g_assert(graph != NULL);
  g_assert(GRDF_IS_GRAPH(graph));
  g_assert(str != NULL);
  g_assert(len != NULL);

  // Allocate a new auto-growing string to write the N3 into
  acc = g_string_new("");
  g_assert(acc != NULL);

  // Walk through the list of all statemets
  stmts = graph->stmts;

  while (stmts != NULL) {
    stmt = GRDF_STMT_CAST(stmts->data);

    grdf_stmt_to_gstr_n3(stmt,acc);

    stmts = g_list_next(stmts);
  }

  // If the string was already allocated, check length and fill it
  if (*str != NULL) {
    // gstring->len doesn't include NULL terminator
    g_assert( (acc->len+1) <= *len);

    strncpy(*str, acc->str, acc->len);
    *len = acc->len+1;

  // Otherwise we need to allocate the string ourselves
  } else {
    *str = g_strndup(acc->str, acc->len);
    *len = acc->len+1;
  }
}
