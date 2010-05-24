/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_NODE_H__
#define __GRDF_NODE_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_NODE (grdf_node_get_type())
#define GRDF_IS_NODE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_NODE))
#define GRDF_IS_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_NODE))
#define GRDF_NODE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_NODE, GrdfNodeClass))
#define GRDF_NODE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_NODE, GrdfNode))
#define GRDF_NODE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_NODE, GrdfNodeClass))
#define GRDF_NODE_CAST(obj) ((GrdfNode*)(obj))

//typedef struct _GrdfNode GrdfNode;
typedef struct _GrdfNodeClass GrdfNodeClass;

struct _GrdfNode
{
  GObject object;
};

struct _GrdfNodeClass
{
  GObjectClass parent_class;

  gboolean (*equal) (GrdfNode *a, GrdfNode *b);
  void (*fprintf) (GrdfNode *node, FILE *f);
  void (*to_gstr_n3) (GrdfNode *node, GString *str);
  void (*to_gstr_xml) (GrdfNode *node, GString *str);
};

GType grdf_node_get_type (void);

#define grdf_node_ref g_object_ref
#define grdf_node_unref g_object_unref

gboolean grdf_node_equal(GrdfNode *a, GrdfNode *b);
void grdf_node_fprintf(GrdfNode *node, FILE *f);
void grdf_node_to_gstr_n3(GrdfNode *node, GString *str);
void grdf_node_to_gstr_xml(GrdfNode *node, GString *str);

G_END_DECLS

#endif /* __GRDF_NODE_H__ */
