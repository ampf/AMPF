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
//static guint grdf_node_variable_signals[LAST_SIGNAL] = { 0 };

static void grdf_node_variable_class_init (GrdfNodeVariableClass * klass);
static void grdf_node_variable_base_class_init (gpointer klass);
static void grdf_node_variable_base_class_finalize (gpointer klass);

static void grdf_node_variable_init (GrdfNodeVariable *node);
static void grdf_node_variable_finalize (GrdfNodeVariable *node);

static void grdf_node_variable_fprintf (GrdfNode *node, FILE *f);
static void grdf_node_variable_to_gstr_n3 (GrdfNode *node, GString *str);
static void grdf_node_variable_to_gstr_xml (GrdfNode *node, GString *str);

GType
grdf_node_variable_get_type (void)
{
  static GType grdf_node_variable_type = 0;

  if (G_UNLIKELY (grdf_node_variable_type == 0)) {
    static const GTypeInfo grdf_node_variable_info = {
      sizeof (GrdfNodeVariableClass),
      grdf_node_variable_base_class_init,
      grdf_node_variable_base_class_finalize,
      (GClassInitFunc) grdf_node_variable_class_init,
      NULL,
      NULL,
      sizeof (GrdfNodeVariable),
      0,
      (GInstanceInitFunc) grdf_node_variable_init,
      NULL
    };

    grdf_node_variable_type = g_type_register_static (GRDF_TYPE_NODE, "GrdfNodeVariable",
	&grdf_node_variable_info, 0);
  }
  return grdf_node_variable_type;
}

static void
grdf_node_variable_class_init (GrdfNodeVariableClass * klass)
{
  GObjectClass *gobject_class;
  GrdfNodeClass *grdf_node_class;

  gobject_class = (GObjectClass *) klass;
  grdf_node_class = (GrdfNodeClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize =
	(GObjectFinalizeFunc)grdf_node_variable_finalize;

  grdf_node_class->fprintf = grdf_node_variable_fprintf;
  grdf_node_class->to_gstr_n3 = grdf_node_variable_to_gstr_n3;
  grdf_node_class->to_gstr_xml = grdf_node_variable_to_gstr_xml;
}

static void
grdf_node_variable_base_class_init (gpointer klass)
{
//  GrdfNodeVariableClass *grdf_node_variable_class = GRDF_NODE_VARIABLE_CLASS (klass);
}

static void
grdf_node_variable_base_class_finalize (gpointer klass)
{
//  GrdfNodeVariableClass *grdf_node_variable_class = GRDF_NODE_VARIABLE_CLASS (klass);
}

static void
grdf_node_variable_init (GrdfNodeVariable *node)
{
}

static void
grdf_node_variable_finalize (GrdfNodeVariable *node)
{
  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(node));
}

GrdfNode *
grdf_node_variable_new (GrdfWorld *world, const gchar *key)
{
  GrdfNodeVariable *var;

  var = g_object_new (GRDF_TYPE_NODE_VARIABLE, NULL);
  g_assert(var != NULL);

  var->key = g_strdup(key);

  return GRDF_NODE_CAST(var);
}

static void
grdf_node_variable_fprintf (GrdfNode *node, FILE *f) {
  GrdfNodeVariable *var;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_VARIABLE(node));

  var = GRDF_NODE_VARIABLE_CAST(node);

  fprintf(f,"?%s",var->key);
}

static void
grdf_node_variable_to_gstr_n3 (GrdfNode *node, GString *str) {
  GrdfNodeVariable *var;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_VARIABLE(node));

  var = GRDF_NODE_VARIABLE_CAST(node);

  g_string_append_printf(str,"?%s",var->key);
}

static void
grdf_node_variable_to_gstr_xml (GrdfNode *node, GString *str) {
  GrdfNodeVariable *var;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_VARIABLE(node));

  var = GRDF_NODE_VARIABLE_CAST(node);

//  fprintf(f,"#%s%d",anon->key,anon->seq);
}
