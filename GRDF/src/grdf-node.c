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
//static guint grdf_node_signals[LAST_SIGNAL] = { 0 };

static void grdf_node_class_init (GrdfNodeClass * klass);
static void grdf_node_base_class_init (gpointer klass);
static void grdf_node_base_class_finalize (gpointer klass);

static void grdf_node_init (GrdfNode *node);
//static void grdf_node_finalize (GrdfNode *node);

GType
grdf_node_get_type (void)
{
  static GType grdf_node_type = 0;

  if (G_UNLIKELY (grdf_node_type == 0)) {
    static const GTypeInfo grdf_node_info = {
      sizeof (GrdfNodeClass),
      grdf_node_base_class_init,
      grdf_node_base_class_finalize,
      (GClassInitFunc) grdf_node_class_init,
      NULL,
      NULL,
      sizeof (GrdfNode),
      0,
      (GInstanceInitFunc) grdf_node_init,
      NULL
    };

    grdf_node_type = g_type_register_static (G_TYPE_OBJECT, "GrdfNode",
  &grdf_node_info, 0);
  }
  return grdf_node_type;
}

static void
grdf_node_class_init (GrdfNodeClass * klass)
{
//  GObjectClass *gobject_class = (GObjectClass *) klass;

//  gobject_class-> finalize =
//	(GObjectFinalizeFunc) grdf_node_finalize;

  parent_class = g_type_class_peek_parent (klass);
}

static void
grdf_node_base_class_init (gpointer klass)
{
//  GrdfNodeClass *grdf_node_class = GRDF_NODE_CLASS (klass);
}

static void
grdf_node_base_class_finalize (gpointer klass)
{
//  GrdfNodeClass *grdf_node_class = GRDF_NODE_CLASS (klass);
}

static void
grdf_node_init (GrdfNode *node)
{
  g_object_force_floating((GObject *)node);
}

//static void
//grdf_node_finalize (GrdfNode *node)
//{
//  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(node));
//}


gboolean
grdf_node_equal(GrdfNode *a, GrdfNode *b)
{
  GrdfNodeClass *class;

  g_assert(a != NULL);
  g_assert(GRDF_IS_NODE(a));

  class = GRDF_NODE_GET_CLASS (a);

  // printf("class=%p, class->equal=%p\n", class, class->equal);

  if (class->equal)
    return class->equal(a,b);
  else if (a == b) return TRUE;
  return FALSE;
}

void
grdf_node_fprintf(GrdfNode *node, FILE *f)
{
  GrdfNodeClass *class;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE(node));

  class = GRDF_NODE_GET_CLASS (node);

  g_assert(class->fprintf != NULL);
  class->fprintf(node,f);
}

void
grdf_node_to_gstr_n3(GrdfNode *node, GString *str)
{
  GrdfNodeClass *class;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE(node));

  class = GRDF_NODE_GET_CLASS (node);

  g_assert(class->to_gstr_n3 != NULL);
  class->to_gstr_n3(node,str);
}

void
grdf_node_to_gstr_xml(GrdfNode *node, GString *str)
{
  GrdfNodeClass *class;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE(node));

  class = GRDF_NODE_GET_CLASS (node);

  g_assert(class->to_gstr_n3 != NULL);
  class->to_gstr_n3(node,str);
}
