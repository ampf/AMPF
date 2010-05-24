/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_NODE_URI_H__
#define __GRDF_NODE_URI_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_NODE_URI (grdf_node_uri_get_type())
#define GRDF_IS_NODE_URI(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_NODE_URI))
#define GRDF_IS_NODE_URI_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_NODE_URI))
#define GRDF_NODE_URI_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_NODE_URI, GrdfNodeUriClass))
#define GRDF_NODE_URI(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_NODE_URI, GrdfNodeUri))
#define GRDF_NODE_URI_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_NODE_URI, GrdfNodeUriClass))
#define GRDF_NODE_URI_CAST(obj) ((GrdfNodeUri*)(obj))

//typedef struct _GrdfNodeUri GrdfNodeUri;
typedef struct _GrdfNodeUriClass GrdfNodeUriClass;

struct _GrdfNodeUri
{
  GrdfNode node;

  GrdfOntology *ontology;
  gchar *uri;
};

struct _GrdfNodeUriClass
{
  GrdfNodeClass parent_class;

};

GType grdf_node_uri_get_type (void);

GrdfNode *grdf_node_uri_new (GrdfGraph *graph, const gchar *uri);
GrdfNode *grdf_node_uri_new_with_ontology (GrdfGraph *graph, GrdfOntology *ont, const gchar *uri);

gboolean grdf_node_uri_match(GrdfNode *node, const gchar *uri);


G_END_DECLS

#endif /* __GRDF_NODE_URI_H__ */
