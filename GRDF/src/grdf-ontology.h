/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_ONTOLOGY_H__
#define __GRDF_ONTOLOGY_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_ONTOLOGY (grdf_ontology_get_type())
#define GRDF_IS_ONTOLOGY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_ONTOLOGY))
#define GRDF_IS_ONTOLOGY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_ONTOLOGY))
#define GRDF_ONTOLOGY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_ONTOLOGY, GrdfOntologyClass))
#define GRDF_ONTOLOGY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_ONTOLOGY, GrdfOntology))
#define GRDF_ONTOLOGY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_ONTOLOGY, GrdfOntologyClass))
#define GRDF_ONTOLOGY_CAST(obj) ((GrdfOntology*)(obj))

//typedef struct _GrdfOntology GrdfOntology;
typedef struct _GrdfOntologyClass GrdfOntologyClass;

struct _GrdfOntology
{
  GObject object;

  gchar *uri;
};

struct _GrdfOntologyClass
{
  GObjectClass parent_class;

};

GType grdf_ontology_get_type (void);

GrdfOntology *grdf_ontology_new (gchar *uri);

#define grdf_ontology_ref	g_object_ref
#define grdf_ontology_unref	g_object_unref


G_END_DECLS

#endif /* __GRDF_ONTOLOGY_H__ */
