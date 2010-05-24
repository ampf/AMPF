/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __GRDF_TEMPLATE_H__
#define __GRDF_TEMPLATE_H__

#include <grdf.h>

G_BEGIN_DECLS

#define GRDF_TYPE_TEMPLATE (grdf_template_get_type())
#define GRDF_IS_TEMPLATE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GRDF_TYPE_TEMPLATE))
#define GRDF_IS_TEMPLATE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GRDF_TYPE_TEMPLATE))
#define GRDF_TEMPLATE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GRDF_TYPE_TEMPLATE, GrdfTemplateClass))
#define GRDF_TEMPLATE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GRDF_TYPE_TEMPLATE, GrdfTemplate))
#define GRDF_TEMPLATE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GRDF_TYPE_TEMPLATE, GrdfTemplateClass))
#define GRDF_TEMPLATE_CAST(obj) ((GrdfTemplate*)(obj))

//typedef struct _GrdfTemplate GrdfTemplate;
typedef struct _GrdfTemplateClass GrdfTemplateClass;

struct _GrdfTemplate
{
  GObject object;

};

struct _GrdfTemplateClass
{
  GObjectClass parent_class;

};

GType grdf_template_get_type (void);

GrdfTemplate *grdf_template_new (GrdfGraph *graph);


G_END_DECLS

#endif /* __GRDF_TEMPLATE_H__ */
