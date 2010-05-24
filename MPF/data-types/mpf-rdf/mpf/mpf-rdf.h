/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <stdint.h>

#include <mpf/mpf-component.h>
#include <grdf.h>

/**
 * SECTION: mpf-datatypes
 * @short_description:
 */
/**
 * MPF_RDF_CAPS:
 */
#define MPF_RDF_CAPS "application/x-mpf-rdf"

GstStaticCaps mpf_rdf_caps;

G_BEGIN_DECLS

/* --- prototypes --- */
GrdfGraph *mpf_rdf_new ();

void mpf_rdf_add_input (const gchar *name);
void mpf_rdf_add_output (const gchar *name);

GstBuffer *mpf_rdf_get_buffer(MpfComponent *component, GrdfGraph *graph);

/**
 * mpf_rdf_pull:
 * @padname: the name of an input pad to pull from
 *
 * Pull a #GrdfGraph from the input pad, for processing in the component.  A
 * reference is transferred to the component code.
 *
 * Returns: a valid #GrdfGraph
 */
#define mpf_rdf_pull(padname) \
        ((GrdfGraph *)mpf_component_pull(NULL,padname))
/**
 * mpf_rdf_push:
 * @padname: the name of an output pad to push the #IplImage out to
 * @data: the pointer of data to push
 *
 * Send a pointer out the given output pad, to the next element in the pipeline.
 * This takes ownership of a reference.
 */
#define mpf_rdf_push(pad,data) \
        mpf_component_push_buffer(NULL,pad,mpf_rdf_get_buffer(NULL,data))

/**
 * mpf_rdf_push:
 * @padname: the name of an output pad to push the #IplImage out to
 * @data: the pointer of data to push
 *
 * Send a pointer out the given output pad, to the next element in the pipeline.
 * This takes ownership of a reference.  Allows multiple buffers to be sent in a
 * single cycle.
 */
#define mpf_rdf_push_pad(pad,data) \
        mpf_component_push_buffer_pad(NULL,pad,mpf_rdf_get_buffer(NULL,data),NULL)

/**
 * mpf_rdf_ref:
 * @data: the pointer to increment the reference count of
 *
 * Increment the reference count of the pointer, used to take ownership.
 */
#define mpf_rdf_ref(data) mpf_component_data_ref((gpointer)data)
/**
 * mpf_rdf_unref:
 * @image: the #IplImage to decrement the reference count of
 *
 * Decrement the reference count of the pointer.  If the reference count drops to
 * zero, the data will be freed.  After an _unref call, you generally may not
 * assume the pointer still points to valid memory.
 */
#define mpf_rdf_unref(data) mpf_component_data_unref((gpointer)data)

G_END_DECLS
