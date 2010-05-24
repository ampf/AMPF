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
//static guint grdf_node_uri_signals[LAST_SIGNAL] = { 0 };

static void grdf_node_uri_class_init (GrdfNodeUriClass * klass);
static void grdf_node_uri_base_class_init (gpointer klass);
static void grdf_node_uri_base_class_finalize (gpointer klass);

static void grdf_node_uri_init (GrdfNodeUri *node);
static void grdf_node_uri_finalize (GrdfNodeUri *node);

static gboolean grdf_node_uri_equal (GrdfNode *a, GrdfNode *b);
static void grdf_node_uri_fprintf (GrdfNode *node, FILE *f);
static void grdf_node_uri_to_gstr_n3 (GrdfNode *node, GString *str);
static void grdf_node_uri_to_gstr_xml (GrdfNode *node, GString *str);

GType
grdf_node_uri_get_type (void)
{
  static GType grdf_node_uri_type = 0;

  if (G_UNLIKELY (grdf_node_uri_type == 0)) {
    static const GTypeInfo grdf_node_uri_info = {
      sizeof (GrdfNodeUriClass),
      grdf_node_uri_base_class_init,
      grdf_node_uri_base_class_finalize,
      (GClassInitFunc) grdf_node_uri_class_init,
      NULL,
      NULL,
      sizeof (GrdfNodeUri),
      0,
      (GInstanceInitFunc) grdf_node_uri_init,
      NULL
    };

    grdf_node_uri_type = g_type_register_static (GRDF_TYPE_NODE, "GrdfNodeUri",
  &grdf_node_uri_info, 0);
  }
  return grdf_node_uri_type;
}

static void
grdf_node_uri_class_init (GrdfNodeUriClass * klass)
{
  GObjectClass *gobject_class;
  GrdfNodeClass *grdf_node_class;

  gobject_class = (GObjectClass *) klass;
  grdf_node_class = (GrdfNodeClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  grdf_node_class->equal = grdf_node_uri_equal;

  gobject_class->finalize =
  (GObjectFinalizeFunc)grdf_node_uri_finalize;

  grdf_node_class->fprintf = grdf_node_uri_fprintf;
  grdf_node_class->to_gstr_n3 = grdf_node_uri_to_gstr_n3;
  grdf_node_class->to_gstr_xml = grdf_node_uri_to_gstr_xml;
}

static void
grdf_node_uri_base_class_init (gpointer klass)
{
//  GrdfNodeUriClass *grdf_node_uri_class = GRDF_NODE_URI_CLASS (klass);
}

static void
grdf_node_uri_base_class_finalize (gpointer klass)
{
//  GrdfNodeUriClass *grdf_node_uri_class = GRDF_NODE_URI_CLASS (klass);
}

static void
grdf_node_uri_init (GrdfNodeUri *node)
{
}

static void
grdf_node_uri_finalize (GrdfNodeUri *node)
{
  if (node->uri)
    g_free(node->uri);

  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(node));
}


GrdfNode *
grdf_node_uri_new_canonical (GrdfGraph *graph, const gchar *uri)
{
  GrdfNodeUri *node;

  node = g_object_new (GRDF_TYPE_NODE_URI, NULL);

  node->uri = g_strdup(uri);

  return (GrdfNode *)node;
}

GrdfNode *
grdf_node_uri_new_with_ontology (GrdfGraph *graph, GrdfOntology *ont, const gchar *uri)
{
  GrdfNodeUri *node;

  node = g_object_new (GRDF_TYPE_NODE_URI, NULL);

  g_object_ref(ont);
  node->ontology = ont;

  node->uri = g_strdup(uri);

  return (GrdfNode *)node;
}

GrdfNode *
grdf_node_uri_new(GrdfGraph *graph, const gchar *uri)
{
  return grdf_node_uri_new_canonical(graph,uri);
}


static gboolean
grdf_node_uri_equal (GrdfNode *a, GrdfNode *b)
{
  g_assert(a != NULL);
  g_assert(GRDF_IS_NODE_URI(a));
  g_assert(b != NULL);
  g_assert(GRDF_IS_NODE_URI(b));
  return !strcmp(((GrdfNodeUri*)a)->uri, ((GrdfNodeUri*)b)->uri);
}

static void
grdf_node_uri_fprintf (GrdfNode *node, FILE *f) {
  GrdfNodeUri *uri;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_URI(node));

  uri = GRDF_NODE_URI_CAST(node);

  fprintf(f,"#%s",uri->uri);
}

static void
grdf_node_uri_to_gstr_n3 (GrdfNode *node, GString *str) {
  GrdfNodeUri *uri;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_URI(node));

  uri = GRDF_NODE_URI_CAST(node);

  // FIXME needs to deal with prefixes
  g_string_append_printf(str,"<%s>",uri->uri);
}

static void
grdf_node_uri_to_gstr_xml (GrdfNode *node, GString *str) {
  GrdfNodeUri *uri;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_URI(node));

  uri = GRDF_NODE_URI_CAST(node);

//  fprintf(f,"#%s",uri->uri);
}

gboolean
grdf_node_uri_match(GrdfNode *node, const gchar *uri)
{
  GrdfNodeUri *urinode;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_URI(node));

  urinode = GRDF_NODE_URI(node);

  if (strcmp(urinode->uri, uri) == 0)
    return TRUE;
  else
    return FALSE;
}

