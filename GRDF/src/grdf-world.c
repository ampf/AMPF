/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <glib.h>

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
//static guint grdf_world_signals[LAST_SIGNAL] = { 0 };

static void grdf_world_class_init (GrdfWorldClass *klass, gpointer class_data);
static void grdf_world_base_class_init (gpointer klass);
static void grdf_world_base_class_finalize (gpointer klass);

static void grdf_world_init (GrdfWorld *world);
static void grdf_world_finalize (GrdfWorld *world);

GType
grdf_world_get_type (void)
{
  static GType grdf_world_type = 0;

  if (G_UNLIKELY (grdf_world_type == 0)) {
    static const GTypeInfo grdf_world_info = {
      sizeof (GrdfWorldClass),
      grdf_world_base_class_init,
      grdf_world_base_class_finalize,
      (GClassInitFunc) grdf_world_class_init,
      NULL,
      NULL,
      sizeof (GrdfWorld),
      0,
      (GInstanceInitFunc) grdf_world_init,
      NULL
    };

    grdf_world_type = g_type_register_static (G_TYPE_OBJECT, "GrdfWorld",
	&grdf_world_info, 0);
  }
  return grdf_world_type;
}

static void
grdf_world_class_init (GrdfWorldClass *klass, gpointer class_data)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  gobject_class->finalize =
	(GObjectFinalizeFunc) grdf_world_finalize;
}

static void
grdf_world_base_class_init (gpointer klass)
{
//  GrdfWorldClass *grdf_world_class = GRDF_WORLD_CLASS (klass);
}

static void
grdf_world_base_class_finalize (gpointer klass)
{
//  GrdfWorldClass *grdf_world_class = GRDF_WORLD_CLASS (klass);
}

static void
grdf_world_init (GrdfWorld *world)
{
  // Create all the hash tables, including the free functions needed
  world->ontologies = g_hash_table_new_full(g_str_hash, g_str_equal,
	g_free, grdf_ontology_unref);
  world->nodecache_uri = g_hash_table_new_full(g_str_hash, g_str_equal,
//	g_free, grdf_node_uri_unref);
	g_free, NULL);	// FIXME
  world->nodecache_anon = g_hash_table_new_full(g_str_hash, g_int_equal,
	g_free, g_free);
}

static void
grdf_world_finalize (GrdfWorld *world)
{
//  fprintf(stderr,"in grdf_world_finalize\n");
  // Destroy all the hash tables, which will call the appropriate free/unref
  g_hash_table_destroy(world->ontologies);
  g_hash_table_destroy(world->nodecache_uri);
  g_hash_table_destroy(world->nodecache_anon);

  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(world));
}

/**
 * grdf_world_new:
 *
 * Create a new RDF world structure.
 *
 * Returns: new #GrdfWorld
 */
GrdfWorld *
grdf_world_new ()
{
  GrdfWorld *world;

  // Just because.  We should check to see if it isn't init'd already and print a warning...
  grdf_init();

  world = g_object_new (GRDF_TYPE_WORLD, NULL);

  return world;
}

/**
 * grdf_world_add_ontology:
 * @world: the world to add an ontology to
 * @ont: the ontology to add
 * @prefix: a string prefix for the ontology
 *
 * Add the given #GrdfOntology to the #GrdfWorld, with a prefix
 * that allows for shortened URIs that refer to that ontology.
 */
void
grdf_world_add_ontology(GrdfWorld *world, GrdfOntology *ont, const gchar *prefix)
{
  gchar *prefix_copy;

  g_assert(world != NULL);
  g_assert(GRDF_IS_WORLD(world));
  g_assert(ont != NULL);
  g_assert(GRDF_IS_ONTOLOGY(ont));
  g_assert(prefix != NULL);

  g_object_ref_sink(ont);
  prefix_copy = g_strdup(prefix);

  g_hash_table_insert(world->ontologies, (gpointer)prefix_copy, (gpointer)ont);
}
