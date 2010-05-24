/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_NODE_LITERAL_H__
#define __GRDF_NODE_LITERAL_H__

G_BEGIN_DECLS

#include <grdf.h>

#define GRDF_TYPE_NODE_LITERAL (grdf_node_literal_get_type())
#define GRDF_IS_NODE_LITERAL(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_NODE_LITERAL))
#define GRDF_IS_NODE_LITERAL_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_NODE_LITERAL))
#define GRDF_NODE_LITERAL_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_NODE_LITERAL, GrdfNodeLiteralClass))
#define GRDF_NODE_LITERAL(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_NODE_LITERAL, GrdfNodeLiteral))
#define GRDF_NODE_LITERAL_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_NODE_LITERAL, GrdfNodeLiteralClass))
#define GRDF_NODE_LITERAL_CAST(obj) ((GrdfNodeLiteral*)(obj))

//typedef struct _GrdfNodeLiteral GrdfNodeLiteral;
typedef struct _GrdfNodeLiteralClass GrdfNodeLiteralClass;

struct _GrdfNodeLiteral
{
  GrdfNode node;

  GValue *value;
};

struct _GrdfNodeLiteralClass
{
  GrdfNodeClass parent_class;

};

GType grdf_node_literal_get_type (void);

GrdfNode *grdf_node_literal_new_gvalue (GrdfGraph *graph, GValue *val);
GrdfNode *grdf_node_literal_new_string (GrdfGraph *graph, const gchar *val);
GrdfNode *grdf_node_literal_new_int (GrdfGraph *graph, gint val);
GrdfNode *grdf_node_literal_new_uint64 (GrdfGraph *graph, guint64 val);
GrdfNode *grdf_node_literal_new_float (GrdfGraph *graph, gfloat val);

gint grdf_node_literal_get_int(GrdfNode *node);
void grdf_node_literal_set_int(GrdfNode *node, gint value);


G_END_DECLS

#endif /* __GRDF_NODE_LITERAL_H__ */
