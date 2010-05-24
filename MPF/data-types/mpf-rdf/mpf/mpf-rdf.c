/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/**
 * SECTION: mpf-datatypes
 * @short_description:
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdint.h>
#include <gst/gst.h>

#include "mpf-rdf.h"
#include <mpf/mpf-compound-buffer.h>

GstStaticCaps mpf_rdf_caps = GST_STATIC_CAPS ( MPF_RDF_CAPS );

static GrdfWorld *
_mpf_rdf_get_world()
{
  static GrdfWorld *world = NULL;

  if (!world) {
    world = grdf_world_new();
  }

  return world;
}

/**
 * mpf_rdf_add_input:
 * @name: name of the input pad to add
 *
 * Adds an input pad to the component of the given name, supporting RDF metadata.
 */
void
mpf_rdf_add_input (const gchar *name)
{
  /* FIXME this should add ontologies and all sorts of other stuff */
  mpf_component_add_input_with_capstr(NULL,name,"application/x-mpf-rdf");
}

/**
 * mpf_rdf_add_output:
 * @name: name of the output pad to add
 *
 * Adds an output pad to the component of the given name, for RDF metadata.
 */
void
mpf_rdf_add_output (const gchar *name)
{
  /* FIXME this should add ontologies and all sorts of other stuff */
  mpf_component_add_output_with_capstr(NULL,name,"application/x-mpf-rdf");
}

/**
 * mpf_rdf_new:
 *
 * Creates a new empty graph.  This graph lives in the MPF-RDF "RDF world".
 *
 * Returns: a new #RdfGraph.
 */
GrdfGraph *
mpf_rdf_new ()
{
  MpfComponent *component = mpf_component_get_curcomponent();
  GrdfWorld *world;
  GrdfGraph *graph;

  world = _mpf_rdf_get_world();
  graph = grdf_graph_new(world);

  return graph;
}

static void
mpf_rdf_finalize_buffer(MpfCompoundBuffer *buf)
{
  GrdfGraph *graph;

  graph = (GrdfGraph *)GST_BUFFER_DATA(buf);
  grdf_graph_unref(graph);
}

/**
 * mpf_rdf_get_buffer:
 * @component: a valid #MpfComponent.
 * @graph: a valid #GrdfGraph.
 *
 * Returns an existing #GstBuffer or adds a new #GstBuffer that is
 * associated with the #GrdfGraph.
 *
 * Returns: a valid #GstBuffer.
 */
GstBuffer *
mpf_rdf_get_buffer(MpfComponent *component, GrdfGraph *graph)
{
  GstBuffer *buf;

  if (component == NULL)
    component = mpf_component_get_curcomponent();

  /* If the buffer already exists for this data, return it */
  if ((buf = mpf_component_find_buffer(component,(gpointer)graph)))
    return buf;

  /* Otherwise we need to make a new one */
#if 1
  buf = gst_buffer_new();
  gst_buffer_set_data(buf,(guint8 *)graph,sizeof(GrdfGraph));
#else
  buf = mpf_compound_buffer_new(graph,sizeof(GrdfGraph),mpf_rdf_finalize_buffer,NULL,NULL);
//mpf_rdf_copy_buffer);
#endif

  /* Add it to the component's private list */
  mpf_component_list_buffer(component, buf);

  return buf;
}
