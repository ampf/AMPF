/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_NODE_ANON_H__
#define __GRDF_NODE_ANON_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_NODE_ANON (grdf_node_anon_get_type())
#define GRDF_IS_NODE_ANON(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_NODE_ANON))
#define GRDF_IS_NODE_ANON_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_NODE_ANON))
#define GRDF_NODE_ANON_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_NODE_ANON, GrdfNodeAnonClass))
#define GRDF_NODE_ANON(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_NODE_ANON, GrdfNodeAnon))
#define GRDF_NODE_ANON_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_NODE_ANON, GrdfNodeAnonClass))
#define GRDF_NODE_ANON_CAST(obj) ((GrdfNodeAnon*)(obj))

//typedef struct _GrdfNodeAnon GrdfNodeAnon;
typedef struct _GrdfNodeAnonClass GrdfNodeAnonClass;

struct _GrdfNodeAnon
{
  GrdfNode object;

  gchar *key;
  gint seq;
};

struct _GrdfNodeAnonClass
{
  GrdfNodeClass parent_class;

};

GType grdf_node_anon_get_type (void);

GrdfNode *grdf_node_anon_new (GrdfGraph *graph, const gchar *key);


G_END_DECLS

#endif /* __GRDF_NODE_ANON_H__ */
