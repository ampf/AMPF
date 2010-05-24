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
//static guint grdf_node_literal_signals[LAST_SIGNAL] = { 0 };

static void grdf_node_literal_class_init (GrdfNodeLiteralClass * klass);
static void grdf_node_literal_base_class_init (gpointer klass);
static void grdf_node_literal_base_class_finalize (gpointer klass);

static void grdf_node_literal_init (GrdfNodeLiteral *node);
static void grdf_node_literal_finalize (GrdfNodeLiteral *node);

static void grdf_node_literal_fprintf (GrdfNode *node, FILE *f);
static void grdf_node_literal_to_gstr_n3 (GrdfNode *node, GString *str);
static void grdf_node_literal_to_gstr_xml (GrdfNode *node, GString *str);

GType
grdf_node_literal_get_type (void)
{
  static GType grdf_node_literal_type = 0;

  if (G_UNLIKELY (grdf_node_literal_type == 0)) {
    static const GTypeInfo grdf_node_literal_info = {
      sizeof (GrdfNodeLiteralClass),
      grdf_node_literal_base_class_init,
      grdf_node_literal_base_class_finalize,
      (GClassInitFunc) grdf_node_literal_class_init,
      NULL,
      NULL,
      sizeof (GrdfNodeLiteral),
      0,
      (GInstanceInitFunc) grdf_node_literal_init,
      NULL
    };

    grdf_node_literal_type = g_type_register_static (GRDF_TYPE_NODE, "GrdfNodeLiteral",
  &grdf_node_literal_info, 0);
  }
  return grdf_node_literal_type;
}

static void
grdf_node_literal_class_init (GrdfNodeLiteralClass * klass)
{
  GObjectClass *gobject_class;
  GrdfNodeClass *grdf_node_class;

  gobject_class = (GObjectClass *) klass;
  grdf_node_class = (GrdfNodeClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize =
  (GObjectFinalizeFunc)grdf_node_literal_finalize;

  grdf_node_class->fprintf = grdf_node_literal_fprintf;
  grdf_node_class->to_gstr_n3 = grdf_node_literal_to_gstr_n3;
  grdf_node_class->to_gstr_xml = grdf_node_literal_to_gstr_xml;
}

static void
grdf_node_literal_base_class_init (gpointer klass)
{
//  GrdfNodeLiteralClass *grdf_node_literal_class = GRDF_NODE_LITERAL_CLASS (klass);
}

static void
grdf_node_literal_base_class_finalize (gpointer klass)
{
//  GrdfNodeLiteralClass *grdf_node_literal_class = GRDF_NODE_LITERAL_CLASS (klass);
}

static void
grdf_node_literal_init (GrdfNodeLiteral *node)
{
}

static void
grdf_node_literal_finalize (GrdfNodeLiteral *node)
{
  if (node->value)
    g_free(node->value);

  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(node));
}


GrdfNode *
grdf_node_literal_new_gvalue (GrdfGraph *graph, GValue *val)
{
  GrdfNodeLiteral *lit;

  // FIXME need to check for an existing literal in the world

  // Allocate new literal node object
  lit = g_object_new (GRDF_TYPE_NODE_LITERAL, NULL);
  g_assert(lit != NULL);

  // Allocate a new GValue and copy the argument into it
  lit->value = g_new0(GValue, 1);
  g_value_copy(val, lit->value);

  // Return it cast to GrdfNode
  return GRDF_NODE_CAST(lit);
}

GrdfNode *
grdf_node_literal_new_string (GrdfGraph *graph, const gchar *val)
{
  GrdfNodeLiteral *lit;

  // FIXME need to check for an existing literal in the world

  // Allocate new literal node object
  lit = g_object_new (GRDF_TYPE_NODE_LITERAL, NULL);
  g_assert(lit != NULL);

  // Allocate a new GValue and set the given argument
  lit->value = g_new0(GValue, 1);
  g_value_init(lit->value, G_TYPE_STRING);
  g_value_set_string(lit->value, val);

  // Return it cast to GrdfNode
  return GRDF_NODE_CAST(lit);
}

GrdfNode *
grdf_node_literal_new_int (GrdfGraph *graph, gint val)
{
  GrdfNodeLiteral *lit;

  // FIXME need to check for an existing literal in the world

  // Allocate new literal node object
  lit = g_object_new (GRDF_TYPE_NODE_LITERAL, NULL);
  g_assert(lit != NULL);

  // Allocate a new GValue and set the given argument
  lit->value = g_new0(GValue, 1);
  g_value_init(lit->value, G_TYPE_INT);
  g_value_set_int(lit->value, val);

  // Return it cast to GrdfNode
  return GRDF_NODE_CAST(lit);
}

GrdfNode *
grdf_node_literal_new_uint64 (GrdfGraph *graph, guint64 val)
{
  GrdfNodeLiteral *lit;

  // FIXME need to check for an existing literal in the world

  // Allocate new literal node object
  lit = g_object_new (GRDF_TYPE_NODE_LITERAL, NULL);
  g_assert(lit != NULL);

  // Allocate a new GValue and set the given argument
  lit->value = g_new0(GValue, 1);
  g_value_init(lit->value, G_TYPE_UINT64);
  g_value_set_uint64(lit->value, val);

  // Return it cast to GrdfNode
  return GRDF_NODE_CAST(lit);
}


GrdfNode *
grdf_node_literal_new_float (GrdfGraph *graph, gfloat val)
{
  GrdfNodeLiteral *lit;

  // FIXME need to check for an existing literal in the world

  // Allocate new literal node object
  lit = g_object_new (GRDF_TYPE_NODE_LITERAL, NULL);
  g_assert(lit != NULL);

  // Allocate a new GValue and set the given argument
  lit->value = g_new0(GValue, 1);
  g_value_init(lit->value, G_TYPE_FLOAT);
  g_value_set_float(lit->value, val);

  // Return it cast to GrdfNode
  return GRDF_NODE_CAST(lit);
}

static void
grdf_node_literal_fprintf (GrdfNode *node, FILE *f) {
  GrdfNodeLiteral *lit;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_LITERAL(node));

  lit = GRDF_NODE_LITERAL_CAST(node);

  if (G_VALUE_TYPE(lit->value) == G_TYPE_STRING)
    fprintf(f,"'%s'",g_value_get_string(lit->value));
  else if (G_VALUE_TYPE(lit->value) == G_TYPE_INT)
    fprintf(f,"%d",g_value_get_int(lit->value));
  else if (G_VALUE_TYPE(lit->value) == G_TYPE_UINT64)
    fprintf(f,"%llu",(unsigned long long)g_value_get_uint64(lit->value));
  else if (G_VALUE_TYPE(lit->value) == G_TYPE_FLOAT)
    fprintf(f,"%f",g_value_get_float(lit->value));
}

static void
grdf_node_literal_to_gstr_n3 (GrdfNode *node, GString *str) {
  GrdfNodeLiteral *lit;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_LITERAL(node));

  lit = GRDF_NODE_LITERAL_CAST(node);

  if (G_VALUE_TYPE(lit->value) == G_TYPE_STRING)
    g_string_append_printf(str,"\"%s\"",g_value_get_string(lit->value));
  else if (G_VALUE_TYPE(lit->value) == G_TYPE_INT)
    g_string_append_printf(str,"\"%d\"",g_value_get_int(lit->value));
  else if (G_VALUE_TYPE(lit->value) == G_TYPE_UINT64)
    g_string_append_printf(str,"\"%llu\"",(unsigned long long)g_value_get_uint64(lit->value));
  else if (G_VALUE_TYPE(lit->value) == G_TYPE_FLOAT)
    g_string_append_printf(str,"\"%f\"", g_value_get_float(lit->value));
}

static void
grdf_node_literal_to_gstr_xml (GrdfNode *node, GString *str) {
  GrdfNodeLiteral *lit;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_LITERAL(node));

  lit = GRDF_NODE_LITERAL_CAST(node);

//  if (G_VALUE_TYPE(lit->value) == G_TYPE_STRING)
//    fprintf(f,"'%s'",g_value_get_string(lit->value));
//  else if (G_VALUE_TYPE(lit->value) == G_TYPE_INT)
//    fprintf(f,"%d",g_value_get_int(lit->value));
}


gint
grdf_node_literal_get_int(GrdfNode *node)
{
  GrdfNodeLiteral *lit;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_LITERAL(node));

  lit = GRDF_NODE_LITERAL_CAST(node);
  g_assert(G_VALUE_TYPE(lit->value) == G_TYPE_INT);

  return g_value_get_int(lit->value);
}

void
grdf_node_literal_set_int(GrdfNode *node, gint value)
{
  GrdfNodeLiteral *lit;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_LITERAL(node));

  lit = GRDF_NODE_LITERAL_CAST(node);
  g_assert(G_VALUE_TYPE(lit->value) == G_TYPE_INT);

  g_value_set_int(lit->value, value);

}
