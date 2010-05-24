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
//static guint grdf_node_anon_signals[LAST_SIGNAL] = { 0 };

static void grdf_node_anon_class_init (GrdfNodeAnonClass * klass);
static void grdf_node_anon_base_class_init (gpointer klass);
static void grdf_node_anon_base_class_finalize (gpointer klass);

static void grdf_node_anon_init (GrdfNodeAnon *node);
static void grdf_node_anon_finalize (GrdfNodeAnon *node);

static void grdf_node_anon_fprintf (GrdfNode *node, FILE *f);
static void grdf_node_anon_to_gstr_n3 (GrdfNode *node, GString *str);
static void grdf_node_anon_to_gstr_xml (GrdfNode *node, GString *str);

static gboolean grdf_node_anon_equal (GrdfNode *a, GrdfNode *b);


struct _GrdfNodeAnonCacheData {
  gint seq;
  gint refcount;
};

GType
grdf_node_anon_get_type (void)
{
  static GType grdf_node_anon_type = 0;

  if (G_UNLIKELY (grdf_node_anon_type == 0)) {
    static const GTypeInfo grdf_node_anon_info = {
      sizeof (GrdfNodeAnonClass),
      grdf_node_anon_base_class_init,
      grdf_node_anon_base_class_finalize,
      (GClassInitFunc) grdf_node_anon_class_init,
      NULL,
      NULL,
      sizeof (GrdfNodeAnon),
      0,
      (GInstanceInitFunc) grdf_node_anon_init,
      NULL
    };

    grdf_node_anon_type = g_type_register_static (GRDF_TYPE_NODE, "GrdfNodeAnon",
  &grdf_node_anon_info, 0);
  }
  return grdf_node_anon_type;
}

static void
grdf_node_anon_class_init (GrdfNodeAnonClass * klass)
{
  GObjectClass *gobject_class;
  GrdfNodeClass *grdf_node_class;

  gobject_class = (GObjectClass *) klass;
  grdf_node_class = (GrdfNodeClass *) klass;

  parent_class = g_type_class_peek_parent (klass);

  grdf_node_class->equal = grdf_node_anon_equal;

  gobject_class->finalize =
  (GObjectFinalizeFunc)grdf_node_anon_finalize;

  grdf_node_class->fprintf = grdf_node_anon_fprintf;
  grdf_node_class->to_gstr_n3 = grdf_node_anon_to_gstr_n3;
  grdf_node_class->to_gstr_xml = grdf_node_anon_to_gstr_xml;
}

static void
grdf_node_anon_base_class_init (gpointer klass)
{
//  GrdfNodeAnonClass *grdf_node_anon_class = GRDF_NODE_ANON_CLASS (klass);
}

static void
grdf_node_anon_base_class_finalize (gpointer klass)
{
//  GrdfNodeAnonClass *grdf_node_anon_class = GRDF_NODE_ANON_CLASS (klass);
}

static void
grdf_node_anon_init (GrdfNodeAnon *node)
{
}

static void
grdf_node_anon_finalize (GrdfNodeAnon *node)
{
  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(node));
}

static gboolean
grdf_node_anon_equal (GrdfNode *a, GrdfNode *b)
{
  g_assert(a != NULL);
  g_assert(GRDF_IS_NODE_ANON(a));
  g_assert(b != NULL);
  g_assert(GRDF_IS_NODE_ANON(b));
  return !strcmp(((GrdfNodeAnon*)a)->key, ((GrdfNodeAnon*)b)->key);
}

static void dump_nodecache_anon(GHashTable *t) __attribute__ ((unused));
static void dump_nodecache_anon(GHashTable *t) {
  GHashTableIter iter;
  gchar *key;
  struct _GrdfNodeAnonCacheData *cache;

  fprintf(stderr,"nodecache_anon:(%p)\n",t);
  g_hash_table_iter_init (&iter, t);
  while (g_hash_table_iter_next (&iter, (gpointer)&key, (gpointer)&cache))
  {
    fprintf(stderr,"    '%s'(%p): seq:%d ref:%d\n",key,key,cache->seq,cache->refcount);
  }
}

GrdfNode *
grdf_node_anon_new (GrdfGraph *graph, const gchar *key)
{
  GrdfWorld *world = graph->world;
  gchar *storedkey;
  GrdfNodeAnon *anon;
  struct _GrdfNodeAnonCacheData *cache;
  gint sequence;

//  fprintf(stderr,"GRDF: creating new anon key '%s'\n",key);

  // If given a world, try to find an existing sequence.
  if (world != NULL) {
//    fprintf(stderr,"GRDF: have a world %p!\n",world);
    g_assert(world->nodecache_anon != NULL);

//    dump_nodecache_anon(world->nodecache_anon);

    // If the key already exists...
    if (g_hash_table_lookup_extended(world->nodecache_anon, key, (gpointer)&storedkey, (gpointer)&cache)) {
//      fprintf(stderr,"key exists at %p, sequence is %d\n",storedkey, cache->seq);
      // Increment the sequence and add to the refcount
      sequence = ++cache->seq;
      cache->refcount++;
    // but if it doesn't, add it
    } else {
      storedkey = g_strdup(key);
      cache = g_new0(struct _GrdfNodeAnonCacheData, 1);
      sequence = cache->seq = 0;
      cache->refcount = 1;
//      fprintf(stderr,"GRDF: new key, storing with key %p\n",storedkey);
      g_hash_table_insert(world->nodecache_anon, storedkey, cache);
    }
  } else {
    // FIXME need to deal with this case
    g_assert(world != NULL);
  }

  // Now allocate the anon object and fill it all in
  anon = g_object_new (GRDF_TYPE_NODE_ANON, NULL);
  g_assert(anon != NULL);

  anon->key = storedkey;
  anon->seq = sequence;

  return GRDF_NODE_CAST(anon);
}

static void
grdf_node_anon_fprintf (GrdfNode *node, FILE *f) {
  GrdfNodeAnon *anon;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_ANON(node));

  anon = GRDF_NODE_ANON_CAST(node);

  fprintf(f,"#%s%d",anon->key,anon->seq);
}

static void
grdf_node_anon_to_gstr_n3 (GrdfNode *node, GString *str) {
  GrdfNodeAnon *anon;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_ANON(node));

  anon = GRDF_NODE_ANON_CAST(node);

  g_string_append_printf(str,"_:%s%d",anon->key,anon->seq);
}

static void
grdf_node_anon_to_gstr_xml (GrdfNode *node, GString *str) {
  GrdfNodeAnon *anon;

  g_assert(node != NULL);
  g_assert(GRDF_IS_NODE_ANON(node));

  anon = GRDF_NODE_ANON_CAST(node);

//  fprintf(f,"#%s%d",anon->key,anon->seq);
}
