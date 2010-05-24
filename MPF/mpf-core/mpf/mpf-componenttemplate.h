/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/*
 * Expects the following to be present in the file containing this template:
 *
 * #define COMPONENT_NAME "nameofyourcomponent"
 * #define COMPONENT_DESC "Describe your component"
 * #define COMPONENT_AUTH "Your Name <you@somewhere.com>"
 * struct component_private { };
 * static void component_class_init(MpfComponentClass *klass) { }
 * static void component_init(MpfComponent *component) { }
 * static void component_process(MpfComponent *component) { }
 *
 * Optional #defines
 * #define MPF_COMPONENT_NAMESPACE "Mpf"
 * #define COMPONENT_TYPE "Filter/Analytics"
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <mpf/mpf-component.h>

/**
 * MPF_COMPONENT_PRIVATE:
 * @obj: a valid #Template
 *
 * Returns: <structname>component_private</structname> structure
 */
#define MPF_COMPONENT_PRIVATE(obj) (((Template *)obj)->priv)
#define mpf_private (MPF_COMPONENT_PRIVATE(component))

typedef struct _Template      Template;
typedef struct _TemplateClass TemplateClass;


struct _Template
{
  MpfComponent component;

  /*< private >*/
  struct component_private priv;
};

struct _TemplateClass
{
  MpfComponentClass parent_class;
};

static GObjectClass *parent_class = NULL;

static void template_class_init (TemplateClass *klass, gpointer class_data);
//static void template_class_finalize (TemplateClass *klass, gpointer class_data);

static void template_init (Template *object, TemplateClass *kclass);
static void template_finalize (Template *object);

#if !defined(MPF_COMPONENT_NAMESPACE)
#define MPF_COMPONENT_NAMESPACE "Appscio"
#endif

#if !defined(COMPONENT_TYPE)
#define COMPONENT_TYPE "Filter/Analytics"
#endif

static GType template_get_type(void)
{
  static GType template_type = 0;

  if (G_UNLIKELY (template_type == 0)) {
    static const GTypeInfo template_info = {
      sizeof(TemplateClass),
      NULL,
      NULL,
      (GClassInitFunc) template_class_init,
//      (GClassFinalizeFunc) template_class_finalize,
      NULL,
      NULL,
      sizeof (Template),
      0,
      (GInstanceInitFunc) template_init,
    };

    template_type = g_type_register_static (MPF_TYPE_COMPONENT,
        MPF_COMPONENT_NAMESPACE "-" COMPONENT_NAME,
        &template_info,
        (GTypeFlags) 0);
  }
  return template_type;
}

static void
template_class_init (TemplateClass *klass, gpointer class_data)
{
  GObjectClass *gobject_class = (GObjectClass *) klass;
  static GstElementDetails element_details = {
    (char *)COMPONENT_NAME,
    (char *)COMPONENT_TYPE,
    (char *)COMPONENT_DESC,
    (char *)COMPONENT_AUTH
  };

  parent_class = (GObjectClass *)g_type_class_peek_parent (klass);

  gobject_class->finalize =
  (GObjectFinalizeFunc) template_finalize;

  gst_element_class_set_details (GST_ELEMENT_CLASS(klass), &element_details);

  // Temporarily set the threadlocal klass pointer and clear the component pointer
  mpf_component_set_curklass((MpfComponentClass *)klass);
  mpf_component_set_curcomponent((MpfComponent *)NULL);

  // Call the component's main class initialization routine
  // This makes pad templates, parameter templates, etc.
  if (__component_class_init) (__component_class_init)((MpfComponentClass *)klass);

  // Now we call the parent class's base_init
  mpf_component_class_init((MpfComponentClass *)klass);

  // We set the component's main process function to component_process
  mpf_component_set_process((MpfComponentClass *)klass, __component_process);

  // Set all the other functions
  mpf_component_set_setup((MpfComponentClass *)klass, __component_setup);
  mpf_component_set_teardown((MpfComponentClass *)klass, __component_teardown);
}

/* NOT USED BY STATIC CLASS
static void
template_class_finalize(TemplateClass *klass, gpointer class_data)
{
  if (__component_class_finalize) (__component_class_finalize)((MpfComponentClass *)klass);
}
*/

static void
template_init (Template *object, TemplateClass *klass)
{
  MpfComponent *component = (MpfComponent *)object;

//  fprintf(stderr,"+++ template_init(object=%p, klass=%p)\n",object,klass);

  component->priv = &(object->priv);

  // Set the threadlocal component pointer and clear the klass pointer
  mpf_component_set_curklass(NULL);
  mpf_component_set_curcomponent(component);

  // Call the component's per-instance init routine
  if (__component_init) (__component_init)(component);
}

static void
template_finalize (Template *object)
{
  MpfComponent *component = (MpfComponent *)object;

  // Call the component's per-instance finalize routine
  if (__component_finalize) (__component_finalize)(component);

  G_OBJECT_CLASS (parent_class)->finalize (G_OBJECT(object));
}

/**
 * plugin_init:
 * @plugin: a #GstPlugin
 *
 * The entry point to initialize the @plugin itself, register the element factories,
 * and pad templates.
 */
static gboolean
plugin_init (GstPlugin *plugin)
{
  /* exchange the strings 'plugin' and 'Template plugin' with your
   * plugin name and description */
//  GST_DEBUG_CATEGORY_INIT (gst_fromiplimage_debug, "fromiplimage",
//      0, "Convert to IplImage");
  return gst_element_register (plugin, COMPONENT_NAME, GST_RANK_NONE, template_get_type());
}

/* This is the structure that gstreamer looks for to register a #GstPlugin */
GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    COMPONENT_NAME,
    COMPONENT_DESC,
    plugin_init, VERSION, "LGPL", "GStreamer", "http://gstreamer.net/")
