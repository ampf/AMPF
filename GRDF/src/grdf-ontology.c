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
//static guint grdf_ontology_signals[LAST_SIGNAL] = { 0 };

static void grdf_ontology_class_init (GrdfOntologyClass * klass);
static void grdf_ontology_base_class_init (gpointer klass);
static void grdf_ontology_base_class_finalize (gpointer klass);
static void grdf_ontology_init (GrdfOntology * grdf_ontology);

GType
grdf_ontology_get_type (void)
{
  static GType grdf_ontology_type = 0;

  if (G_UNLIKELY (grdf_ontology_type == 0)) {
    static const GTypeInfo grdf_ontology_info = {
      sizeof (GrdfOntologyClass),
      grdf_ontology_base_class_init,
      grdf_ontology_base_class_finalize,
      (GClassInitFunc) grdf_ontology_class_init,
      NULL,
      NULL,
      sizeof (GrdfOntology),
      0,
      (GInstanceInitFunc) grdf_ontology_init,
      NULL
    };

    grdf_ontology_type = g_type_register_static (G_TYPE_OBJECT, "GrdfOntology",
	&grdf_ontology_info, 0);
  }
  return grdf_ontology_type;
}

static void
grdf_ontology_class_init (GrdfOntologyClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_peek_parent (klass);
}

static void
grdf_ontology_base_class_init (gpointer klass)
{
//  GrdfOntologyClass *grdf_ontology_class = GRDF_ONTOLOGY_CLASS (klass);
}

static void
grdf_ontology_base_class_finalize (gpointer klass)
{
//  GrdfOntologyClass *grdf_ontology_class = GRDF_ONTOLOGY_CLASS (klass);
}

static void
grdf_ontology_init (GrdfOntology * grdf_ontology)
{
  g_object_force_floating ((GObject *)grdf_ontology);
}


GrdfOntology *
grdf_ontology_new (gchar *uri)
{
  GrdfOntology *ontology;

  ontology = g_object_new (GRDF_TYPE_ONTOLOGY, NULL);
  ontology->uri = g_strdup(uri);

  return ontology;
}
