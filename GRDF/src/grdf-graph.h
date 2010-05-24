/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_GRAPH_H__
#define __GRDF_GRAPH_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_GRAPH (grdf_graph_get_type())
#define GRDF_IS_GRAPH(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_GRAPH))
#define GRDF_IS_GRAPH_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_GRAPH))
#define GRDF_GRAPH_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_GRAPH, GrdfGraphClass))
#define GRDF_GRAPH(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_GRAPH, GrdfGraph))
#define GRDF_GRAPH_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_GRAPH, GrdfGraphClass))
#define GRDF_GRAPH_CAST(obj) ((GrdfGraph*)(obj))

//typedef struct _GrdfGraph GrdfGraph;
typedef struct _GrdfGraphClass GrdfGraphClass;

struct _GrdfGraph
{
  GObject object;

  GrdfWorld *world;

  GList *stmts;
  gint numstmts;
};

struct _GrdfGraphClass
{
  GObjectClass parent_class;

};

GType grdf_graph_get_type (void);

GrdfGraph *grdf_graph_new (GrdfWorld *world);

#define grdf_graph_ref(graph) g_object_ref(graph)
#define grdf_graph_unref(graph) g_object_unref(graph)

GrdfGraph *grdf_graph_copy(GrdfGraph *graph);
GrdfGraph *grdf_graph_make_writable(GrdfGraph *graph);

void grdf_graph_add_stmt (GrdfGraph *graph, GrdfStmt *stmt);

GList *grdf_stmtlist_find_subj_node(GrdfStmtList *stmts, GrdfNode *subj);
GList *grdf_stmtlist_find_subj_uri(GrdfStmtList *stmts, const gchar *subj);
GList *grdf_stmtlist_find_pred_node(GrdfStmtList *stmts, GrdfNode *pred);
GList *grdf_stmtlist_find_pred_uri(GrdfStmtList *stmts, const gchar *pred);
GList *grdf_stmtlist_find_obj_node(GrdfStmtList *stmts, GrdfNode *obj);
GList *grdf_stmtlist_find_obj_uri(GrdfStmtList *stmts, const gchar *obj);
GList *grdf_stmtlist_find_ctx(GrdfStmtList *stmts, GrdfStmt *ctx);

GList *grdf_graph_find_subj_node(GrdfGraph *graph, GrdfNode *subj);
GList *grdf_graph_find_subj_uri(GrdfGraph *graph, const gchar *subj);
GList *grdf_graph_find_pred_node(GrdfGraph *graph, GrdfNode *pred);
GList *grdf_graph_find_pred_uri(GrdfGraph *graph, const gchar *pred);
GList *grdf_graph_find_obj_node(GrdfGraph *graph, GrdfNode *obj);
GList *grdf_graph_find_obj_uri(GrdfGraph *graph, const gchar *obj);
GList *grdf_graph_find_ctx(GrdfGraph *graph, GrdfStmt *ctx);

GrdfStmt *grdf_stmtlist_find_first_subj_node(GrdfStmtList *stmts, GrdfNode *subj);
GrdfStmt *grdf_stmtlist_find_first_subj_uri(GrdfStmtList *stmts, const gchar *subj);
GrdfStmt *grdf_stmtlist_find_first_pred_node(GrdfStmtList *stmts, GrdfNode *pred);
GrdfStmt *grdf_stmtlist_find_first_pred_uri(GrdfStmtList *stmts, const gchar *pred);
GrdfStmt *grdf_stmtlist_find_first_obj_node(GrdfStmtList *stmts, GrdfNode *obj);
GrdfStmt *grdf_stmtlist_find_first_obj_uri(GrdfStmtList *stmts, const gchar *obj);
GrdfStmt *grdf_stmtlist_find_first_ctx(GrdfStmtList *stmts, GrdfStmt *ctx);

GrdfStmt *grdf_graph_find_first_subj_node(GrdfGraph *graph, GrdfNode *subj);
GrdfStmt *grdf_graph_find_first_subj_uri(GrdfGraph *graph, const gchar *subj);
GrdfStmt *grdf_graph_find_first_pred_node(GrdfGraph *graph, GrdfNode *pred);
GrdfStmt *grdf_graph_find_first_pred_uri(GrdfGraph *graph, const gchar *pred);
GrdfStmt *grdf_graph_find_first_obj_node(GrdfGraph *graph, GrdfNode *obj);
GrdfStmt *grdf_graph_find_first_obj_uri(GrdfGraph *graph, const gchar *obj);
GrdfStmt *grdf_graph_find_first_ctx(GrdfGraph *graph, GrdfStmt *ctx);

void grdf_graph_unref_stmtlist(GrdfGraph *graph, GList *nodelist);

void grdf_graph_fprintf(FILE *f,GrdfGraph *graph);
void grdf_graph_dump(FILE *f,GrdfGraph *graph);

gint grdf_graph_num_stmt(GrdfGraph *graph);
GrdfStmt *grdf_graph_get_stmt(GrdfGraph *graph, gint index);


void grdf_graph_to_n3(GrdfGraph *graph,gchar **str,gint *len);


G_END_DECLS

#endif /* __GRDF_GRAPH_H__ */
