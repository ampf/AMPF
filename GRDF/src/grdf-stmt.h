/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_STMT_H__
#define __GRDF_STMT_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_STMT (grdf_stmt_get_type())
#define GRDF_IS_STMT(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_STMT))
#define GRDF_IS_STMT_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_STMT))
#define GRDF_STMT_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_STMT, GrdfStmtClass))
#define GRDF_STMT(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_STMT, GrdfStmt))
#define GRDF_STMT_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_STMT, GrdfStmtClass))
#define GRDF_STMT_CAST(obj) ((GrdfStmt*)(obj))

//typedef struct _GrdfStmt GrdfStmt;
typedef struct _GrdfStmtClass GrdfStmtClass;

struct _GrdfStmt
{
  GObject object;

  GrdfNode *subj, *pred, *obj;
  GrdfStmt *ctx;
};

struct _GrdfStmtClass
{
  GObjectClass parent_class;

};

GType grdf_stmt_get_type (void);

GrdfStmt *grdf_stmt_new(GrdfGraph *graph, GrdfNode *subj, GrdfNode *pred, GrdfNode *obj);
GrdfStmt *grdf_stmt_new_ctx(GrdfGraph *graph, GrdfNode *subj, GrdfNode *pred, GrdfNode *obj, GrdfStmt *ctx);

/* n = node, u = uri, a = anon, l = literal string, i = int, ui64 = uint64, f = float */
#define grdf_stmt_new_nun(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    subj,				\
    grdf_node_uri_new(graph,pred),	\
    obj)

#define grdf_stmt_new_nuu(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    subj,				\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_uri_new(graph,obj))

#define grdf_stmt_new_nul(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    subj,				\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_literal_new_string(graph,obj))

#define grdf_stmt_new_nui(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    subj,				\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_literal_new_int(graph,obj))

#define grdf_stmt_new_nuui64(graph,subj,pred,obj)  \
  grdf_stmt_new(graph,              \
        subj,               \
        grdf_node_uri_new(graph,pred),  \
        grdf_node_literal_new_uint64(graph,obj))

#define grdf_stmt_new_nuf(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    subj,				\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_literal_new_float(graph,obj))


#define grdf_stmt_new_uun(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    grdf_node_uri_new(graph,subj),	\
    grdf_node_uri_new(graph,pred),	\
    obj)

#define grdf_stmt_new_uuu(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    grdf_node_uri_new(graph,subj),	\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_uri_new(graph,obj))

#define grdf_stmt_new_uua(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    grdf_node_uri_new(graph,subj),	\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_anon_new(graph,obj))

#define grdf_stmt_new_uul(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    grdf_node_uri_new(graph,subj),	\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_literal_new_string(graph,obj))

#define grdf_stmt_new_uui(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    grdf_node_uri_new(graph,subj),	\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_literal_new_int(graph,obj))

#define grdf_stmt_new_uuf(graph,subj,pred,obj)	\
  grdf_stmt_new(graph,				\
    grdf_node_uri_new(graph,subj),	\
    grdf_node_uri_new(graph,pred),	\
    grdf_node_literal_new_float(graph,obj))

#define grdf_stmt_ref(stmt) g_object_ref(stmt)
#define grdf_stmt_unref(stmt) g_object_unref(stmt)

void grdf_stmt_fprintf(GrdfStmt *stmt, FILE *f);
void grdf_stmt_to_gstr_n3(GrdfStmt *stmt, GString *str);

G_END_DECLS

#endif /* __GRDF_STMT_H__ */
