/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
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
//static guint grdf_template_signals[LAST_SIGNAL] = { 0 };

static void grdf_template_class_init (GrdfTemplateClass * klass);
static void grdf_template_base_class_init (gpointer klass);
static void grdf_template_base_class_finalize (gpointer klass);
static void grdf_template_init (GrdfTemplate * grdf_template);

GType
grdf_template_get_type (void)
{
  static GType grdf_template_type = 0;

  if (G_UNLIKELY (grdf_template_type == 0)) {
    static const GTypeInfo grdf_template_info = {
      sizeof (GrdfTemplateClass),
      grdf_template_base_class_init,
      grdf_template_base_class_finalize,
      (GClassInitFunc) grdf_template_class_init,
      NULL,
      NULL,
      sizeof (GrdfTemplate),
      0,
      (GInstanceInitFunc) grdf_template_init,
      NULL
    };

    grdf_template_type = g_type_register_static (G_TYPE_OBJECT, "GrdfTemplate",
	&grdf_template_info, 0);
  }
  return grdf_template_type;
}

static void
grdf_template_class_init (GrdfTemplateClass * klass)
{
  GObjectClass *gobject_class;

  gobject_class = (GObjectClass *) klass;

  parent_class = g_type_class_peek_parent (klass);
}

static void
grdf_template_base_class_init (gpointer klass)
{
//  GrdfTemplateClass *grdf_template_class = GRDF_TEMPLATE_CLASS (klass);
}

static void
grdf_template_base_class_finalize (gpointer klass)
{
//  GrdfTemplateClass *grdf_template_class = GRDF_TEMPLATE_CLASS (klass);
}

static void
grdf_template_init (GrdfTemplate * grdf_template)
{
}


GrdfTemplate *
grdf_template_new (GrdfGraph *graph)
{
  return g_object_new (GRDF_TYPE_TEMPLATE, NULL);
}
