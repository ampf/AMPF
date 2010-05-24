/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_NODE_VARIABLE_H__
#define __GRDF_NODE_VARIABLE_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_NODE_VARIABLE (grdf_node_variable_get_type())
#define GRDF_IS_NODE_VARIABLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_NODE_VARIABLE))
#define GRDF_IS_NODE_VARIABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_NODE_VARIABLE))
#define GRDF_NODE_VARIABLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_NODE_VARIABLE, GrdfNodeVariableClass))
#define GRDF_NODE_VARIABLE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_NODE_VARIABLE, GrdfNodeVariable))
#define GRDF_NODE_VARIABLE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_NODE_VARIABLE, GrdfNodeVariableClass))
#define GRDF_NODE_VARIABLE_CAST(obj) ((GrdfNodeVariable*)(obj))

//typedef struct _GrdfNodeVariable GrdfNodeVariable;
typedef struct _GrdfNodeVariableClass GrdfNodeVariableClass;

struct _GrdfNodeVariable
{
  GrdfNode object;

  gchar *key;
  gint seq;
};

struct _GrdfNodeVariableClass
{
  GrdfNodeClass parent_class;

};

GType grdf_node_variable_get_type (void);

GrdfNode *grdf_node_variable_new (GrdfWorld *world, const gchar *key);


G_END_DECLS

#endif /* __GRDF_NODE_VARIABLE_H__ */
