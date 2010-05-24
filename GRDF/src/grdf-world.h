/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_WORLD_H__
#define __GRDF_WORLD_H__

#include <glib-object.h>
#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_WORLD (grdf_world_get_type())
#define GRDF_IS_WORLD(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_WORLD))
#define GRDF_IS_WORLD_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_WORLD))
#define GRDF_WORLD_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_WORLD, GrdfWorldClass))
#define GRDF_WORLD(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_WORLD, GrdfWorld))
#define GRDF_WORLD_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_WORLD, GrdfWorldClass))
#define GRDF_WORLD_CAST(obj) ((GrdfWorld*)(obj))

//typedef struct _GrdfWorld GrdfWorld;
typedef struct _GrdfWorldClass GrdfWorldClass;

struct _GrdfWorld
{
  GObject object;

  GHashTable *ontologies;

  GHashTable *nodecache_uri;
  GHashTable *nodecache_anon;
  GHashTable *nodecache_literal;
};

struct _GrdfWorldClass
{
  GObjectClass parent_class;

};

GType grdf_world_get_type (void);

GrdfWorld *grdf_world_new ();

#define grdf_world_ref		g_object_ref
#define grdf_world_unref	g_object_unref

void grdf_world_add_ontology(GrdfWorld *world, GrdfOntology *ont, const gchar *prefix);


G_END_DECLS

#endif /* __GRDF_WORLD_H__ */
