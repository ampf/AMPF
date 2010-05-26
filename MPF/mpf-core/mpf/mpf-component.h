/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifndef __MPF_COMPONENT_H__
#define __MPF_COMPONENT_H__

#include <gst/gst.h>

#define MPF_TYPE_COMPONENT (mpf_component_get_type())
#define MPF_COMPONENT(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),MPF_TYPE_COMPONENT,MpfComponent))
#define MPF_COMPONENT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),MPF_TYPE_COMPONENT,MpfComponentClass))
#define MPF_IS_COMPONENT(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),MPF_TYPE_COMPONENT))
#define MPF_IS_COMPONENT_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),MPF_TYPE_COMPONENT))

#define mpf_debug(comp,...) G_STMT_START{				\
        if ((comp && (comp->flags&MPF_DEBUG))) {				\
            fprintf(stdout,"thread %p: %s: ", g_thread_self(), GST_OBJECT_CAST(comp)->name);			\
            fprintf(stdout, __VA_ARGS__);					\
        }									\
}G_STMT_END

#define LOCK()   mpf_debug(component, "LOCK name=%s, mutex=%p\n", GST_OBJECT_CAST(component)->name, component->mutex); g_mutex_lock(component->mutex)
#define UNLOCK() mpf_debug(component, "UNLOCK name=%s, mutex=%p\n", GST_OBJECT_CAST(component)->name, component->mutex); g_mutex_unlock(component->mutex)


/* --- typedefs & structures --- */
/**
 * MpfComponentProcessReturn:
 * @MPF_BAD:
 * @MPF_GOOD:
 *
 * Enum values used for MpfComponentProcessFunc.
 */
typedef enum {
  MPF_BAD,
  MPF_GOOD,
} MpfComponentProcessReturn;

#define MPF_DEBUG	0x00000001
#define MPF_NOCHAIN	0x00000002

typedef struct _MpfComponent      MpfComponent;
typedef struct _MpfComponentClass MpfComponentClass;

G_BEGIN_DECLS

typedef MpfComponentProcessReturn (*MpfComponentProcessFunc) (MpfComponent *component);
typedef void (*MpfComponentSetupFunc) (MpfComponent *comp);	// FIXME should return something
typedef void (*MpfComponentTeardownFunc) (MpfComponent *comp);	// FIXME ditto
typedef void (*MpfComponentRectifyData) (MpfComponent *comp, gpointer data);

struct _MpfComponent
{
  /*< private >*/
  GstElement element;

  GHashTable *buffers;
  GValue **params;
  GHashTable *paramsmap;
  GList *pads;

  GMutex *mutex;

  GstClockTime cur_timestamp;
  GstClockTime cur_duration;
  gint sink_pads_at_curtime;

  guint flags;


  gpointer priv;

};

struct _MpfComponentClass
{
  /*< private >*/
  GstElementClass parent_class;

  GList *padtemplates;
  gint num_sink_pads, num_src_pads;

  GList *parameters;
  gint numparameters;

  MpfComponentProcessFunc processfunc;
  MpfComponentSetupFunc setupfunc;
  MpfComponentTeardownFunc teardownfunc;

  gpointer priv;
};

typedef struct {
  /*< private >*/
  GstBuffer *buf;
  gint refcount;
  gpointer *priv;
} MpfComponentBufferInfo;

typedef struct {
  /* private */
  MpfComponent *component;
  GstPad *pad;

  GList *inbufs;
  GstBuffer *inbuf, *outbuf;

  gpointer type_priv;
  gpointer priv;
} MpfComponentPadInfo;

typedef struct {
  /* private */
  MpfComponent *component;
  gint paramnum;
  GParamSpec *pspec;
} MpfComponentParamInfo;

GType mpf_component_get_type (void);

#define MPF_SINK GST_PAD_SINK
#define MPF_SRC GST_PAD_SRC



/* --- prototypes --- */

/* Ctors */
void mpf_component_class_init (MpfComponentClass *klass);
void mpf_component_init (MpfComponent *component, MpfComponentClass *klass);

/* Setters */
void mpf_component_set_curklass(MpfComponentClass *klass);
void mpf_component_set_curcomponent(MpfComponent *component);

void mpf_component_set_process(MpfComponentClass *klass, MpfComponentProcessFunc func);
void mpf_component_set_setup(MpfComponentClass *klass, MpfComponentSetupFunc func);
void mpf_component_set_teardown(MpfComponentClass *klass, MpfComponentTeardownFunc func);

/* Getters */
MpfComponentClass *mpf_component_get_curklass();
MpfComponent *mpf_component_get_curcomponent();

void mpf_add_param_boolean(const gchar *name,
      const gchar *nick, const gchar *blurb,
      gboolean default_value);
void mpf_add_param_int(const gchar *name,
      const gchar *nick, const gchar *blurb,
      gint minimum, gint maximum, gint default_value);
void mpf_add_param_float(const gchar *name,
      const gchar *nick, const gchar *blurb,
      gfloat minimum, gfloat maximum, gfloat default_value);
void mpf_add_param_string(const gchar *name,
      const gchar *nick, const gchar *blurb,
      const gchar *default_value);
void mpf_add_param_enum(const gchar *name,
      const gchar *nick, const gchar *blurb,
      const gchar *default_value,
      const gchar *enumname1, const gchar *enumnick1,
      ...) G_GNUC_NULL_TERMINATED;

gint mpf_param_get_int(const gchar *name);
gfloat mpf_param_get_float(const gchar *name);
const gchar *mpf_param_get_string(const gchar *name);
gint mpf_param_get_enum(const gchar *name);

void mpf_component_add_input_with_caps (MpfComponentClass *klass, const gchar *padname, GstCaps *caps);
void mpf_component_add_output_with_caps (MpfComponentClass *klass, const gchar *padname, GstCaps *caps);
void mpf_component_add_input_with_capstr (MpfComponentClass *klass, const gchar *padname, const gchar *capstr);
void mpf_component_add_output_with_capstr (MpfComponentClass *klass, const gchar *padname, const gchar *capstr);

void mpf_component_add_caps (MpfComponentClass *klass, const gchar *padname, const gchar *caps);
void mpf_component_remove_caps (MpfComponentClass *klass, const gchar *padname, const gchar *caps);

void mpf_component_add_input(MpfComponentClass *klass, GstPadTemplate *template);

// Identical functionality to add_input, the template controls SRC/SINK nature
#define mpf_component_add_output(klass, template) mpf_component_add_input(klass, template)

void mpf_component_set_caps (MpfComponentClass *klass, const gchar *padname, const
gchar *caps);

GstPadTemplate *mpf_component_get_pad_template (MpfComponentClass *klass,const gchar *padname);

GstBuffer *mpf_component_pull_buffer (MpfComponent *comp, const gchar *padname);

#define mpf_component_pull(comp,padname) \
  (GST_BUFFER_DATA(mpf_component_pull_buffer((comp),(padname))))

gboolean mpf_component_is_linked(MpfComponent *comp, const gchar* padname);

void mpf_component_push_buffer (MpfComponent *comp,
        const gchar *padname, GstBuffer *buf);

GstBuffer *mpf_component_find_buffer(MpfComponent *comp, gpointer data);

void mpf_component_list_buffer(MpfComponent *comp, GstBuffer *buf);

void mpf_component_data_ref(gpointer data);
void mpf_component_data_unref(gpointer data);

void mpf_component_buffer_ref(MpfComponent *comp, GstBuffer *buf);
void mpf_component_buffer_unref(MpfComponent *comp, GstBuffer *buf);

gpointer mpf_component_make_data_writable(gpointer data, MpfComponentRectifyData rect);

GstCaps *mpf_buffer_get_caps(void *ptr);
const gchar *mpf_buffer_get_mimetype(void *ptr);

gint mpf_buffer_get_int(void *ptr, const gchar *field);
void mpf_buffer_set_int(void *ptr, const gchar *field, gint value);

gint mpf_buffer_get_fraction_numerator(void *ptr, const gchar *field);
gint mpf_buffer_get_fraction_denominator(void *ptr, const gchar *field);
void mpf_buffer_set_fraction(void *ptr, const gchar *field, gint numerator, gint denominator);

gfloat mpf_buffer_get_float(void *ptr, const gchar *field);
const gchar *mpf_buffer_get_string(void *ptr, const gchar *field);

void mpf_buffer_set_timestamp(void *ptr,GstClockTime time);
void mpf_buffer_set_duration(void *ptr,GstClockTime time);

GstClockTime mpf_buffer_get_timestamp(void *ptr);
GstClockTime mpf_buffer_get_duration(void *ptr);

void mpf_buffer_set_caps(void *ptr, GstCaps *caps);

guint64 mpf_buffer_get_offset(void *ptr);

#include <mpf/mpf-component-magicfuncs.h>


G_END_DECLS

#endif /* __MPF_COMPONENT_H__ */
