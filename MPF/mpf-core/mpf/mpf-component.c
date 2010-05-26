/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <gst/video/video.h>

#include <string.h>

/**
 * SECTION: mpf-component
 * @short_description:
 *
 */
#include "mpf-component.h"

//GST_DEBUG_CATEGORY_STATIC (mpf_component_debug);
//#define GST_CAT_DEFAULT mpf_component_debug

/* Filter signals and args */
enum
{
    /* FILL ME */
    LAST_SIGNAL
};

enum
{
    ARG_0,
};

static GstObjectClass *parent_class = NULL;

static GStaticPrivate _curklass = G_STATIC_PRIVATE_INIT;
static GStaticPrivate _curcomponent = G_STATIC_PRIVATE_INIT;

static void mpf_component_base_init    (gpointer klass);
static void mpf_component_set_property (GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec);
static void mpf_component_get_property (GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec);

//static gboolean mpf_component_set_cmpf (GstPad * pad, GstCmpf * cmpf);
static GstFlowReturn mpf_component_chain (GstPad * pad, GstBuffer * buf);

static GstStateChangeReturn mpf_component_change_state (GstElement *element, GstStateChange transition);


GType
mpf_component_get_type (void)
{
    static GType mpf_component_type = 0;

    if (!mpf_component_type) {
        static const GTypeInfo mpf_component_info = {
                sizeof (MpfComponentClass),
                (GBaseInitFunc) mpf_component_base_init,
                NULL,
                NULL,
                NULL,
                NULL,
                sizeof (MpfComponent),
                0,
                (GInstanceInitFunc) mpf_component_init,
        };

        /* mpf_debug(NULL,"registering mpf_component\n"); */
        mpf_component_type = g_type_register_static (GST_TYPE_ELEMENT,
                "MpfComponent",
                &mpf_component_info,
                G_TYPE_FLAG_ABSTRACT);
    }
    return mpf_component_type;
}

void
mpf_component_base_init (gpointer klass)
{
    MpfComponentClass *comp_class = MPF_COMPONENT_CLASS(klass);

    /* fprintf(NULL,"+++ mpf_component_base_init(%p)\n",klass); */

    comp_class->padtemplates = NULL;

    comp_class->parameters = NULL;
    comp_class->numparameters = 0;

    comp_class->processfunc = NULL;
}

void
mpf_component_class_init (MpfComponentClass * klass)
{
    GObjectClass *gobject_class;
    GstElementClass *gstelement_class;
    MpfComponentClass *comp_class;
    GList *iter;
    GstPadTemplate *padtemplate;
    MpfComponentParamInfo *pinfo;

    /* fprintf(stderr,"+++ mpf_component_class_init(%p)\n",klass); */

    gobject_class = (GObjectClass *) klass;
    gstelement_class = (GstElementClass *) klass;
    comp_class = (MpfComponentClass *) klass;

    parent_class = g_type_class_peek_parent (klass);

    gstelement_class->change_state = mpf_component_change_state;

    iter = comp_class->padtemplates;
    comp_class->num_sink_pads = 0;
    while (iter) {
        padtemplate = (GstPadTemplate *)(iter->data);
        /*    fprintf(stderr," registering pad template '%s'\n",
        GST_PAD_TEMPLATE_NAME_TEMPLATE(padtemplate));*/
        gst_element_class_add_pad_template(gstelement_class, padtemplate);

        if (GST_PAD_TEMPLATE_DIRECTION (padtemplate) == GST_PAD_SINK)
            comp_class->num_sink_pads++;

        iter = g_list_next(iter);
    }

    gobject_class->set_property = mpf_component_set_property;
    gobject_class->get_property = mpf_component_get_property;

    iter = klass->parameters;
    while (iter) {
        pinfo = (MpfComponentParamInfo *)(iter->data);
        g_assert(pinfo != NULL);

        /*fprintf(stderr, "%p: installing property %d: '%s'\n",
        klass,
        pinfo->paramnum,g_param_spec_get_name(pinfo->pspec)); */
        g_object_class_install_property(gobject_class, pinfo->paramnum, pinfo->pspec);

        iter = g_list_next(iter);
    }
}

void
mpf_component_init (MpfComponent *component, MpfComponentClass *klass)
{
    //  GstElementClass *gstelement_class = GST_ELEMENT_GET_CLASS (component);

    GList *iter;
    GstPad *pad;
    GstPadTemplate *padtemplate;
    MpfComponentPadInfo *padinfo;
    MpfComponentParamInfo *pinfo;

    //fprintf(stderr,"+++ mpf_component_init(%p,%p)\n",component,klass);

    component->cur_timestamp = GST_CLOCK_TIME_NONE;
    component->cur_duration = GST_CLOCK_TIME_NONE;
    component->sink_pads_at_curtime = 0;

    component->mutex = g_mutex_new();

    component->buffers = g_hash_table_new(g_direct_hash, g_direct_equal);

    /* Loop through and instantiate all pads and add them to the element */
    component->pads = NULL;
    iter = klass->padtemplates;
    while (iter) {
        padtemplate = (GstPadTemplate *)(iter->data);

        //    fprintf(stderr,"	creating pad %s\n",GST_PAD_TEMPLATE_NAME_TEMPLATE(padtemplate));

        pad = gst_pad_new_from_template(padtemplate, GST_PAD_TEMPLATE_NAME_TEMPLATE(padtemplate));
        if (GST_PAD_TEMPLATE_DIRECTION(padtemplate) == GST_PAD_SINK) {
            gst_pad_set_chain_function (pad, GST_DEBUG_FUNCPTR(mpf_component_chain));
        }
        padinfo = g_new(MpfComponentPadInfo, 1);
        padinfo->component = component;
        padinfo->pad = pad;
        padinfo->inbufs = NULL;
        padinfo->inbuf = NULL;
        padinfo->outbuf = NULL;
        gst_pad_set_element_private(pad,padinfo);

        gst_element_add_pad (GST_ELEMENT(component), pad);

        iter = g_list_next(iter);
    }

    //fprintf(stderr, "numparameters: %d\n", klass->numparameters);
    /* Loop through the parameters and set up the value table */
    mpf_debug(component, "Initializing parameters\n");
    component->params = g_new0(GValue *, klass->numparameters);
    component->paramsmap = g_hash_table_new(g_str_hash, g_str_equal);
    iter = klass->parameters;
    while (iter) {
        pinfo = (MpfComponentParamInfo *)(iter->data);
        const gchar *paramname = g_param_spec_get_name(pinfo->pspec);
        //fprintf(stderr, "g_param_spec_get_name: %s\n", paramname);
        GValue *value = g_new0(GValue,1);

        component->params[pinfo->paramnum-1] = value;

        /* Construct the value and store it */
        g_value_init(value, G_PARAM_SPEC_VALUE_TYPE(pinfo->pspec));
        g_param_value_set_default(pinfo->pspec,value);

        //fprintf(stderr, "hash insert name: %s value: %p %d\n", paramname, value, g_value_get_int(value));
        g_hash_table_insert(component->paramsmap, (gpointer)paramname, (gpointer)value);

        //GValue *vv = g_hash_table_lookup(component->paramsmap, paramname);
        //fprintf(stderr, "lookup: %s value: %p %s\n", paramname, value, g_type_name(G_VALUE_TYPE(vv)));
        iter = g_list_next(iter);
    }
}

/**
 * mpf_component_set_process:
 * @klass: a valid #MpfComponentClass
 * @func: a valid #MpfComponentProcessFunc
 *
 * Sets #MpfComponentProcessFunc processing function handle.
 */
void
mpf_component_set_process(MpfComponentClass *klass, MpfComponentProcessFunc func)
{
    klass->processfunc = func;
}

void
mpf_component_set_setup(MpfComponentClass *klass, MpfComponentSetupFunc func)
{
    klass->setupfunc = func;
}

void
mpf_component_set_teardown(MpfComponentClass *klass, MpfComponentTeardownFunc func)
{
    klass->teardownfunc = func;
}

static void
mpf_component_set_property (GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec)
{
    MpfComponent *component = MPF_COMPONENT (object);
    GValue *storedval;

    //printf("got set_property for param '%s'\n",pspec->name);

    storedval = component->params[prop_id-1];
    g_value_copy(value, storedval);
}

static void
mpf_component_get_property (GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec)
{
    MpfComponent *component = MPF_COMPONENT (object);
    //  const guchar *paramname;
    GValue *storedval;

    storedval = component->params[prop_id-1];

    g_value_copy(storedval, value);

    /*
  switch (prop_id) {
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
     */
}

static void
mpf_component_get_early_inbuffers(MpfComponent *component)
{
    GstPad *pad;
    GList *iter;
    GstBuffer *buf;
    MpfComponentPadInfo *padinfo;

    mpf_debug(component,"** MPFCORE: early inbuffers\n");

    /* Need to dereference all the buffers at this point! */
    iter = GST_ELEMENT(component)->sinkpads;
    while (iter) {
        pad = (GstPad *)(iter->data);
        padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);

        if (padinfo->inbufs && !padinfo->inbuf) {
            /* Load the first buffer in the list */
            buf = (GstBuffer *)(padinfo->inbufs->data);
            mpf_debug(component, "** MPFCORE: checking timestamp %lld for pad %s buffer\n", (long long int)component->cur_timestamp, gst_pad_get_name(pad));

            /* If it's at the appropriate timestamp... */
            if (GST_BUFFER_TIMESTAMP(buf) == component->cur_timestamp) {
                /* Remove it from the buffer list */
                padinfo->inbufs = g_list_delete_link(padinfo->inbufs, padinfo->inbufs);
                mpf_debug(component, "** MPFCORE: pad %s buffer ready\n", gst_pad_get_name(pad));

                /* Save it to inbuf and count it */
                padinfo->inbuf = buf;
                component->sink_pads_at_curtime++;
                mpf_debug(component, "** MPFCORE: %s sink_pads_at_curtime==%d\n", gst_pad_get_name(pad), component->sink_pads_at_curtime);
            }
        }

        iter = g_list_next(iter);
    }
}

void
mpf_component_send_outbuffers(MpfComponent *component)
{
    GstPad *pad;
    MpfComponentPadInfo *padinfo;
    GList *iter;

    mpf_debug(component,"** MPFCORE: sending output buffers\n");

    iter = GST_ELEMENT(component)->srcpads;
    while (iter) {
        pad = (GstPad *)(iter->data);
        padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);
        //    fprintf(stderr,"pad %p, padinfo %p\n",pad,padinfo);

        if (padinfo->outbuf) {
            //      fprintf(stderr,"padinfo->outbuf is %p\n",padinfo->outbuf);
            GST_BUFFER_TIMESTAMP(padinfo->outbuf) = component->cur_timestamp;
            mpf_debug(component,"** MPFCORE: pushing buffer %p(%p) to pad '%s' (t=%lld)\n",
                    padinfo->outbuf,
                    GST_BUFFER_DATA(padinfo->outbuf),
                    gst_pad_get_name(pad),
                    (long long int)GST_BUFFER_TIMESTAMP(padinfo->outbuf));
            //      fprintf(stderr,"pushing...\n");
            // Valgrind is reporting a thread data-race condition here.
            GstFlowReturn result = gst_pad_push(pad, GST_BUFFER(padinfo->outbuf));

            // What went wrong?
            if (result != GST_FLOW_OK) {
            	fprintf(stderr, "mpf_component_send_outbuffers: component=%s pad=%s result=%s != ok\n", GST_OBJECT_CAST(component)->name, gst_pad_get_name(pad), gst_flow_get_name(result));
            }
            // TODO: remove this hard assertion?
            g_assert(result == GST_FLOW_OK);

            //      fprintf(stderr,"pushed.\n");
            padinfo->outbuf = NULL;
        } else {
        	mpf_debug(component, "** MPFCORE: nothing on pad '%s'\n", gst_pad_get_name(pad));
        }

        iter = g_list_next(iter);
    }

    mpf_debug(component,"** MPFCORE: done sending output buffers\n");
}

static GstClockTime
mpf_component_get_earliest_inbuffer(MpfComponent *component)
{
    GstPad *pad;
    GList *iter;
    GstBuffer *buf;
    MpfComponentPadInfo *padinfo;

    GstClockTime time = GST_CLOCK_TIME_NONE;

    // Look for earliest input buffer to set current time.
    iter = GST_ELEMENT(component)->sinkpads;
    while (iter) {
        pad = (GstPad *)(iter->data);
        padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);

        if (padinfo->inbufs) {
            /* Load the first buffer in the list */
            buf = (GstBuffer *)(padinfo->inbufs->data);

            if (time == GST_CLOCK_TIME_NONE || GST_BUFFER_TIMESTAMP(buf) < time)
                time = GST_BUFFER_TIMESTAMP(buf);

            mpf_debug(component, "** MPFCORE: earliest time=%lld\n", (long long int)time);
        }

        iter = g_list_next(iter);
    }
    return time;
}


/**
 * mpf_component_chain:
 *
 * This function does the actual processing.
 *
 * Timeline for input buffer synchronizaion.
 *
 * input1 0 1      2        6
 * input2     0     1    2    3
 * comp       0     1    2    ?
 */
static GstFlowReturn
mpf_component_chain (GstPad *pad, GstBuffer *buf)
{
    // This function is called synchronized on the pad, but must also be synchronized on the element.

    MpfComponent *component = MPF_COMPONENT (GST_OBJECT_PARENT (pad));
    MpfComponentClass *klass = MPF_COMPONENT_CLASS(G_OBJECT_GET_CLASS(component));
    MpfComponentPadInfo *padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);
    gint process_ret = MPF_GOOD;

    //  fprintf(stderr,"at top of mpf_component_chain!\n");

    if (component->flags & MPF_NOCHAIN) {
        fprintf(stderr,"skipping chain for %s:%s!\n",GST_OBJECT_CAST(component)->name,gst_pad_get_name(pad));
        return GST_FLOW_OK;
    }

    // TODO: this lock is too wide: narrow.
    LOCK();

    mpf_debug(component,"** MPFCORE: got input buffer %p(%p) t=%lld on pad %s\n",
            buf,GST_BUFFER_DATA(buf),(long long int)GST_BUFFER_TIMESTAMP(buf),gst_pad_get_name(pad));

    // If there are no buffers currently waiting, we can set the timestamp
    if (component->sink_pads_at_curtime == 0) {
        mpf_debug(component,"** MPFCORE: first buffer of timestamp=%lld\n",(long long int)GST_BUFFER_TIMESTAMP(buf));
        // component->cur_timestamp = GST_BUFFER_TIMESTAMP(buf);
        component->cur_timestamp = mpf_component_get_earliest_inbuffer(component);
        // Don't let GST_CLOCK_TIME_NONE buffers set the current timestamp.
        if (GST_BUFFER_TIMESTAMP(buf) > 0 && (component->cur_timestamp <= 0 || GST_BUFFER_TIMESTAMP(buf) < component->cur_timestamp)) {
            component->cur_timestamp = GST_BUFFER_TIMESTAMP(buf);
        }

        // Now's also a good time to get any early buffers in place
        mpf_component_get_early_inbuffers(component);
    }
    // First real timestamp sets component current timestamp, unless already set.
    if (component->cur_timestamp == GST_CLOCK_TIME_NONE && GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE) {
    	component->cur_timestamp = GST_BUFFER_TIMESTAMP(buf);
    }

    // If the current buffer is the appropriate timestamp, store it in inbuf
    if (GST_BUFFER_TIMESTAMP(buf) >= component->cur_timestamp && !padinfo->inbuf) {
        padinfo->inbuf = buf;
        component->sink_pads_at_curtime++;
        mpf_debug(component,"** MPFCORE: buffer pad %s is current (%lld)\n", gst_pad_get_name(pad), (long long int)GST_BUFFER_TIMESTAMP(buf));
        // Otherwise store it in the list of future buffers
    } else if (GST_BUFFER_TIMESTAMP(buf) != GST_CLOCK_TIME_NONE && GST_BUFFER_TIMESTAMP(buf) < component->cur_timestamp) {
        mpf_debug(component,"** MPFCORE: buffer pad %s is late! (%lld) < (%lld)\n", gst_pad_get_name(pad), (long long int)GST_BUFFER_TIMESTAMP(buf), (long long int)component->cur_timestamp);
    } else {
        padinfo->inbufs = g_list_append(padinfo->inbufs, buf);
        mpf_debug(component,"** MPFCORE: buffer pad %s is early! (%lld) > (%lld)\n", gst_pad_get_name(pad), (long long int)GST_BUFFER_TIMESTAMP(buf), (long long int)component->cur_timestamp);
    }

    g_assert(component->sink_pads_at_curtime <= klass->num_sink_pads);

    // If we reached the correct number of inputs at this timestamp, run it
    if (component->sink_pads_at_curtime == klass->num_sink_pads) {

        // Reset the count of pads at current timestamp
        component->sink_pads_at_curtime = 0;
        mpf_debug(component,"** MPFCORE: have all inputs, starting processing sequence\n");

        // Set the threadlocal component but not klass
        mpf_component_set_curklass(NULL);
        mpf_component_set_curcomponent(component);

        // Trigger the component's processing routine
        process_ret = klass->processfunc(component);

        // Clear out all the buffers that are old
        mpf_component_send_outbuffers(component);

        mpf_debug(component,"\n");
    } else {
        mpf_debug(component, "** MPFCORE: not enough buffers: %d < %d\n", component->sink_pads_at_curtime, klass->num_sink_pads);
    }
    UNLOCK();


    //  fprintf(stderr,"at bottom of mpf_component_chain!\n");

    if (process_ret == MPF_GOOD)
        return GST_FLOW_OK;
    else
        return GST_FLOW_ERROR;
}


/**
 * mpf_add_param_int:
 * @name: canonical name of the property specified
 * @nick: nick name for the property specified
 * @blurb: description of the property specified
 * @minimum: minimum value for the property specified
 * @maximum: maximum value for the property specified
 * @default_value: default value for the property specified
 *
 * Increments the number of parameters before assignment; allocates and prefills
 * a new #MpfComponentParamInfo structure.  Constructs the #ParamSpecInt instance
 * and stores it.  Finally, adds it to the list of parameters.
 */
void
mpf_add_param_int(const gchar *name,
        const gchar *nick, const gchar *blurb,
        gint minimum, gint maximum, gint default_value)
{
    MpfComponentClass *klass = mpf_component_get_curklass();
    MpfComponentParamInfo *pinfo;

    /* increment the number of parameters before assigning as props are 1-based */
    klass->numparameters++;

    /* allocate and prefill the paraminfo struct */
    pinfo = g_new0(MpfComponentParamInfo, 1);
    g_assert(pinfo != NULL);
    pinfo->paramnum = klass->numparameters;
    //fprintf(stderr,"mpf_add_param_int(): param name '%s' is paramnum %d default: %d\n", name, pinfo->paramnum, default_value);

    if (strchr(name, '_') != NULL) {
        fprintf(stderr, "WARNING: mpf_add_param_int(%s) attempt to add parameter with name containing '_'. Underscore '_' characters are converted to dashes '-'.\n", name);
    }
    /* construct the paramspec and store it */
    pinfo->pspec = g_param_spec_int(name,nick,blurb,minimum,maximum,default_value,G_PARAM_READWRITE);
    g_assert(pinfo->pspec != NULL);
    //fprintf(stderr,"mpf_add_param_int(): g_param_spec_get_name %s\n", g_param_spec_get_name(pinfo->pspec));

    /* add it to the list */
    klass->parameters = g_list_append(klass->parameters, pinfo);
}

/**
 * mpf_add_param_boolean:
 * @name: canonical name of the property specified
 * @nick: nick name for the property specified
 * @blurb: description of the property specified
 * @default_value: default value for the property specified
 *
 * Increments the number of parameters before assignment; allocates and prefills
 * a new #MpfComponentParamInfo structure.  Constructs the #ParamSpecInt instance
 * and stores it.  Finally, adds it to the list of parameters.
 */
void
mpf_add_param_boolean(const gchar *name,
        const gchar *nick, const gchar *blurb,
        gboolean default_value)
{
    MpfComponentClass *klass = mpf_component_get_curklass();
    MpfComponentParamInfo *pinfo;

    /* increment the number of parameters before assigning as props are 1-based */
    klass->numparameters++;

    /* allocate and prefill the paraminfo struct */
    pinfo = g_new0(MpfComponentParamInfo, 1);
    g_assert(pinfo != NULL);
    pinfo->paramnum = klass->numparameters;
    //fprintf(stderr,"mpf_add_param_int(): param name '%s' is paramnum %d default: %d\n", name, pinfo->paramnum, default_value);

    if (strchr(name, '_') != NULL) {
        fprintf(stderr, "WARNING: mpf_add_param_boolean(%s) attempt to add parameter with name containing '_'. Underscore '_' characters are converted to dashes '-'.\n", name);
    }
    /* construct the paramspec and store it */
    pinfo->pspec = g_param_spec_boolean(name,nick,blurb,default_value,G_PARAM_READWRITE);
    g_assert(pinfo->pspec != NULL);
    //fprintf(stderr,"mpf_add_param_boolean(): g_param_spec_get_name %s\n", g_param_spec_get_name(pinfo->pspec));

    /* add it to the list */
    klass->parameters = g_list_append(klass->parameters, pinfo);
}

/**
 * mpf_add_param_float:
 * @name: canonical name of the property specified
 * @nik: nick name for the property specified
 * @blurb: description of the property specified
 * @minimum: minimum value for the property specified
 * @maximum: maximum value for the property specified
 * @default_value: default value for the property specified
 *
 * Increments the number of parameters before assignment; allocates and prefills
 * a new #MpfComponentParamInfo structure.  Constructs the #ParamSpecFloat
 * instance and stores it.  Finally, adds it to the list of parameters.
 */
void
mpf_add_param_float(const gchar *name,
        const gchar *nick, const gchar *blurb,
        gfloat minimum, gfloat maximum, float default_value)
{
    MpfComponentClass *klass = mpf_component_get_curklass();
    MpfComponentParamInfo *pinfo;

    /* increment the number of parameters before assigning as props are 1-based */
    klass->numparameters++;

    /* allocate and prefill the paraminfo struct */
    pinfo = g_new0(MpfComponentParamInfo, 1);
    g_assert(pinfo != NULL);
    pinfo->paramnum = klass->numparameters;
    //fprintf(stderr,"mpf_add_param_float(): param name '%s' is paramnum %d\n",name,pinfo->paramnum);

    if (strchr(name, '_') != NULL) {
        fprintf(stderr, "WARNING: mpf_add_param_float(%s) attempt to add parameter with name containing '_'. Underscore '_' characters are converted to dashes '-'.\n", name);
    }
    /* construct the paramspec and store it */
    pinfo->pspec = g_param_spec_float(name,nick,blurb,minimum,maximum,default_value,G_PARAM_READWRITE);
    g_assert(pinfo->pspec != NULL);

    /* add it to the list */
    klass->parameters = g_list_append(klass->parameters, pinfo);
}

/**
 * mpf_add_param_string:
 * @name: canonical name of the property specified
 * @nik: nick name for the property specified
 * @blurb: description of the property specified
 * @default_value: default value for the property specified
 *
 * Increments the number of parameters before assignment; allocates and prefills
 * a new #MpfComponentParamInfo structure.  Constructs the #ParamSpecString
 * instance and stores it.  Finally, adds it to the list of parameters.
 */
void
mpf_add_param_string(const gchar *name,
        const gchar *nick, const gchar *blurb,
        const gchar *default_value)
{
    MpfComponentClass *klass = mpf_component_get_curklass();
    MpfComponentParamInfo *pinfo;

    /* increment the number of parameters before assigning as props are 1-based */
    klass->numparameters++;

    /* allocate and prefill the paraminfo struct */
    pinfo = g_new0(MpfComponentParamInfo, 1);
    g_assert(pinfo != NULL);
    pinfo->paramnum = klass->numparameters;
    //fprintf(stderr,"mpf_add_param_float(): param name '%s' is paramnum %d\n",name,pinfo->paramnum);

    if (strchr(name, '_') != NULL) {
        fprintf(stderr, "WARNING: mpf_add_param_string(%s) attempt to add parameter with name containing '_'. Underscore '_' characters are converted to dashes '-'.\n", name);
    }
    /* construct the paramspec and store it */
    pinfo->pspec = g_param_spec_string(name,nick,blurb,default_value,G_PARAM_READWRITE);
    g_assert(pinfo->pspec != NULL);

    /* add it to the list */
    klass->parameters = g_list_append(klass->parameters, pinfo);
}

/**
 * mpf_add_param_enum:
 * @name: canonical name of the property specified
 * @nik: nick name for the property specified
 * @blurb: description of the property specified
 * @default_value: default value for the property specified
 * @enumstr1: string name of first enum value (enum value 1)
 * @enumnick1: human-readable nickname of the first enum value
 * @...: remaining enum strings, terminated by a NULL
 *
 * Increments the number of parameters before assignment; allocates and prefills
 * a new #MpfComponentParamInfo structure.  Constructs the #ParamSpecString
 * instance and stores it.  Finally, adds it to the list of parameters.
 */
void
mpf_add_param_enum(const gchar *name,
        const gchar *nick, const gchar *blurb,
        const gchar *default_value,
        const gchar *enumname, const gchar *enumnick, ...)
{
    va_list args;
    gint i;
    GEnumValue *values;

    gchar *enumtypename;
    GType enumtype;

    MpfComponentClass *klass = mpf_component_get_curklass();
    MpfComponentParamInfo *pinfo;
    gint default_value_idx = 0;

    /* increment the number of parameters before assigning as props are 1-based */
    klass->numparameters++;

    /* allocate and prefill the paraminfo struct */
    pinfo = g_new0(MpfComponentParamInfo, 1);
    g_assert(pinfo != NULL);
    pinfo->paramnum = klass->numparameters;

    /* Allocate space for the first value and the NULL terminator */
    values = g_new(GEnumValue, 2);

    /* Trundle through the varargs to fill them in, growing values as needed */
    i = 0;
    values[i].value = i;				// fill in the first
    values[i].value_name = g_strdup(enumname);
    if (strcmp(values[i].value_name, default_value) == 0)
        default_value_idx = i;
    values[i].value_nick = g_strdup(enumnick);
    i++;

    va_start(args, enumnick);			// start at the 1st nick
    enumnick = va_arg(args, const gchar *);	// move to the 2nd name
    while (enumnick) {				// at this point va_walk is a name
        values = g_renew(GEnumValue, values, i+2);
        values[i].value = i;
        values[i].value_name = g_strdup(enumnick);
        if (strcmp(values[i].value_name, default_value) == 0)
            default_value_idx = i;
        enumnick = va_arg(args, const gchar *);	// move to the nick
        values[i].value_nick = g_strdup(enumnick);
        i++;

        enumnick = va_arg(args, const gchar *);	// move to the next name
    }
    va_end(args);
    values[i].value = 0;				// NULL termination
    values[i].value_name = NULL;
    values[i].value_nick = NULL;

    /* Build the relevant Enum type */
    enumtypename = g_strdup_printf("mpf-enum-%s-%s",G_OBJECT_CLASS_NAME(klass),name);
    enumtype = g_enum_register_static(enumtypename,values);

    if (strchr(name, '_') != NULL) {
        fprintf(stderr, "WARNING: mpf_add_param_enum(%s) attempt to add parameter with name containing '_'. Underscore '_' characters are converted to dashes '-'.\n", name);
    }
    /* construct the paramspec and store it */
    pinfo->pspec = g_param_spec_enum(name,nick,blurb,enumtype,default_value_idx,G_PARAM_READWRITE);
    g_assert(pinfo->pspec != NULL);

    /* add it to the list */
    klass->parameters = g_list_append(klass->parameters, pinfo);
}

/**
 * mpf_param_get_boolean:
 * @name: canonical name of the property specified.
 *
 * Get the contents of a %G_TYPE_BOOLEAN #GValue.
 *
 * Returns: a #gboolean.
 */
gboolean
mpf_param_get_boolean(const gchar *name)
{
    MpfComponent *component = mpf_component_get_curcomponent();

    GValue *value;

    value = g_hash_table_lookup(component->paramsmap, name);
    //fprintf(stderr, "get_int: %s value: %p %d\n", name, value, g_value_get_int(value));
    if (value == NULL) {
        fprintf(stderr, "WARNING: mpf_param_get_boolean(%s) does not exist.\n", name);
        return 0;
    }
    return g_value_get_boolean(value);
}

/**
 * mpf_param_get_int:
 * @name: canonical name of the property specified.
 *
 * Get the contents of a %G_TYPE_INT #GValue.
 *
 * Returns: a #gint.
 */
gint
mpf_param_get_int(const gchar *name)
{
    MpfComponent *component = mpf_component_get_curcomponent();

    GValue *value;

    value = g_hash_table_lookup(component->paramsmap, name);
    //fprintf(stderr, "get_int: %s value: %p %d\n", name, value, g_value_get_int(value));
    if (value == NULL) {
        fprintf(stderr, "WARNING: mpf_param_get_int(%s) does not exist.\n", name);
        return 0;
    }
    return g_value_get_int(value);
}

/**
 * mpf_param_get_float:
 * @name: canonical name of the property specified.
 *
 * Get the contents of a %G_TYPE_FLOAT #GValue.
 *
 * Returns: a #gfloat.
 */
gfloat
mpf_param_get_float(const gchar *name)
{
    MpfComponent *component = mpf_component_get_curcomponent();
    GValue *value;

    value = g_hash_table_lookup(component->paramsmap, name);
    if (value == NULL) {
        fprintf(stderr, "WARNING: mpf_param_get_int(%s) does not exist.\n", name);
        return 0.;
    }
    return g_value_get_float(value);
}

/**
 * mpf_param_get_string:
 * @name: canonical name of the property specified.
 *
 * Get the contents of a %G_TYPE_STRING #GValue.
 *
 * Returns: a #gchar*.
 */
const gchar *
mpf_param_get_string(const gchar *name)
{
    MpfComponent *component = mpf_component_get_curcomponent();
    GValue *value;

    value = g_hash_table_lookup(component->paramsmap, name);
    if (value == NULL) {
        fprintf(stderr, "WARNING: mpf_param_get_string(%s) does not exist.\n", name);
        return NULL;
    }
    return g_value_get_string(value);
}

gint
mpf_param_get_enum(const gchar *name) {
    MpfComponent *component = mpf_component_get_curcomponent();
    GValue *gvalue;

    gvalue = g_hash_table_lookup(component->paramsmap, name);
    return g_value_get_enum(gvalue);
}

/**
 * mpf_component_set_curklass:
 * @klass: a valid #MpfComponentClass.
 *
 *
 */
void mpf_component_set_curklass(MpfComponentClass *klass) {
    g_static_private_set(&_curklass, klass, NULL);
}

/**
 * mpf_component_set_curcomponent:
 * @component: a valid #MpfComponent.
 *
 *
 */
void mpf_component_set_curcomponent(MpfComponent *component) {
    g_static_private_set(&_curcomponent, component, NULL);
}

/**
 * mpf_component_get_curklass:
 *
 * Returns: a valid #MpfComponentClass.
 */
MpfComponentClass *mpf_component_get_curklass() {
    return g_static_private_get(&_curklass);
}

/**
 * mpf_component_get_curcomponent:
 *
 * Returns: a valid #MpfComponent.
 */
MpfComponent *mpf_component_get_curcomponent() {
    return g_static_private_get(&_curcomponent);
}

/**
 * mpf_component_pull_buffer:
 * @comp: a valid #MpfComponent
 * @padname: the name of a pad
 *
 * Returns: a valid #GstBuffer.
 */
GstBuffer *
mpf_component_pull_buffer (MpfComponent *comp, const gchar *padname)
{
    GstPad *pad;
    MpfComponentPadInfo *padinfo;
    GstBuffer *buf;

    if (comp == NULL) comp = mpf_component_get_curcomponent();
    g_assert(comp != NULL);		    /* paranoid! */
    g_assert(MPF_IS_COMPONENT(comp));	/* paranoid! */

    pad = gst_element_get_pad(GST_ELEMENT(comp),padname);
    if (pad == NULL) {
        g_error("pad '%s' doesn't exist!",padname);
        return NULL;
    }
    g_assert(GST_IS_PAD(pad));		/* paranoid! */

    padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);
    g_assert(padinfo != NULL);		/* paranoid! */

    /* Get the buffer from the holding cell */
    buf = padinfo->inbuf;
    if (buf == NULL) {
        g_warning("padinfo for %s:'%s' doesn't have a buffer (%lld), returning NULL",gst_element_get_name(comp), padname, (long long int)comp->cur_timestamp);
        return NULL;
    }

    /* If the buffer's caps aren't set, pull them from the pad */
    if (GST_BUFFER_CAPS(buf) == NULL) {
        //    fprintf(stderr,"buffer doesn't have caps set, pulling in from the pad '%s.%s'\n",GST_ELEMENT_NAME(comp),padname);
        gst_buffer_set_caps(buf,GST_PAD_CAPS(padinfo->pad));
        if (GST_BUFFER_CAPS(buf) == NULL) {
            //      fprintf(stderr,"buffer *still* doesn't have caps set...?\n");
        }
    }

    /* Empty out the holding cell */
    padinfo->inbuf = NULL;

    /* Add it to the component's hash/refcount table */
    mpf_component_list_buffer(comp, buf);

    return buf;
}

/**
 * mpf_component_push_buffer:
 * @comp: a valid #MpfComponent
 * @padname:
 * @buf: a #GstBuffer
 *
 */
void
mpf_component_push_buffer (MpfComponent *comp, const gchar *padname, GstBuffer *buf)
{
    GstPad *pad;
    MpfComponentPadInfo *padinfo;

    if (comp == NULL)
        comp = mpf_component_get_curcomponent();
    g_assert(comp != NULL);
    g_assert(MPF_IS_COMPONENT(comp));

    pad = gst_element_get_pad(GST_ELEMENT(comp),padname);
    g_assert(pad != NULL);
    g_assert(GST_IS_PAD(pad));

    padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);
    g_assert(pad != NULL);

    padinfo->outbuf = buf;
}


/**
 * mpf_component_push_buffer_pad: Push a buffer to a buffer allocated by the downstream
 * component.
 * @comp: a valid #MpfComponent
 * @padname:
 * @buf: a #GstBuffer
 *
 */
void
mpf_component_push_buffer_pad (MpfComponent *comp, const gchar *padname, GstBuffer *buf, const gchar *capspad)
{
    GstPad *pad;
    GstBuffer *outbuf;
    GstFlowReturn result;
    MpfComponentPadInfo *padinfo;
    GstCaps *caps;

    // printf("mpf_component_push_buffer_pad (%s) buf->size=%d\n", padname, GST_BUFFER_SIZE(buf));

    if (comp == NULL)
        comp = mpf_component_get_curcomponent();
    g_assert(comp != NULL);
    g_assert(MPF_IS_COMPONENT(comp));

    pad = gst_element_get_pad(GST_ELEMENT(comp),padname);
    g_assert(pad != NULL);
    g_assert(GST_IS_PAD(pad));

    padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);
    g_assert(pad != NULL);

    caps = capspad? GST_PAD_CAPS(gst_element_get_pad(GST_ELEMENT(comp), capspad)): buf->caps;

    result = gst_pad_alloc_buffer(pad, buf->timestamp,
            buf->size, caps, &outbuf);
    g_assert(result == GST_FLOW_OK);

    //fprintf(stderr, "gst_pad_alloc_buffer pad=%s buf->size=%d returned %d outbuf=%p timestamp=%lld\n", padname, buf->size, result, outbuf, (long long int)buf->timestamp);

    memcpy(outbuf->data, buf->data, buf->size);
    outbuf->size = buf->size;
    outbuf->timestamp = buf->timestamp;

    gst_pad_push(pad, outbuf);
}


/**
 * mpf_component_list_buffer:
 * @comp: a valid #MpfComponent.
 * @buf: a valid #GstBuffer.
 *
 * Creates a new #MpfComponentBufferInfo and adds it to the #MpfComponent instance.
 */
void
mpf_component_list_buffer (MpfComponent *comp, GstBuffer *buf)
{
    MpfComponentBufferInfo *info = g_new(MpfComponentBufferInfo,1);

    info->buf = buf;
    info->refcount = 1;

    /* Add the buffer info and its data pointer to the hash */
    g_hash_table_insert(comp->buffers,GST_BUFFER_DATA(buf),info);
}

/**
 * mpf_component_data_ref:
 * @data: the @data pointer.
 *
 * References the @data.
 */
void
mpf_component_data_ref (gpointer data)
{
    MpfComponent *comp = mpf_component_get_curcomponent();
    MpfComponentBufferInfo *info;

    /* See if there's a buffer associated with this image */
    info = (MpfComponentBufferInfo *)g_hash_table_lookup(comp->buffers, data);

    /* If there is one, reference the buffer and increment refcount */
    if (info) {
        gst_buffer_ref(info->buf);
        info->refcount++;
    }
}

/**
 * mpf_component_data_unref:
 * @data: the @data pointer.
 *
 * Unreferences the @data.
 */
void
mpf_component_data_unref (gpointer data)
{
    MpfComponent *comp = mpf_component_get_curcomponent();
    MpfComponentBufferInfo *info;
    GstBuffer *buf;

    /* See if there's a buffer associated with this image */
    info = (MpfComponentBufferInfo *)g_hash_table_lookup(comp->buffers, data);

    /* If there is one, decrement refcount and forget if needed, then unref buffer */
    if (info) {
        buf = info->buf;
        info->refcount--;
        if (info->refcount == 0) {
            g_hash_table_remove(comp->buffers,data);
            g_free(info);
        }
        gst_buffer_unref(buf);
    }
}

/**
 * mpf_component_buffer_ref:
 * @comp: a valid #MpfComponent.
 * @buf: a valid #GstBuffer.
 *
 * References the @buf.
 */
void
mpf_component_buffer_ref (MpfComponent *comp, GstBuffer *buf)
{
    MpfComponentBufferInfo *info;

    /* See if there's a buffer associated with this image */
    info = (MpfComponentBufferInfo *)g_hash_table_lookup(comp->buffers, GST_BUFFER_DATA(buf));

    /* If there is one, reference the buffer and increment refcount */
    if (info) {
        gst_buffer_ref(buf);
        info->refcount++;
    }
}

/**
 * mpf_component_buffer_unref:
 * @comp: a valid #MpfComponent.
 * @buf: a valid #GstBuffer.
 *
 * Unreferences teh @buf.
 */
void
mpf_component_buffer_unref (MpfComponent *comp, GstBuffer *buf)
{
    MpfComponentBufferInfo *info;

    /* See if there's a buffer associated with this image */
    info = (MpfComponentBufferInfo *)g_hash_table_lookup(comp->buffers, GST_BUFFER_DATA(buf));

    /* If there is one, decrement refcount and forget if needed, then unref */
    if (info) {
        buf = info->buf;
        info->refcount--;
        if (info->refcount == 0) {
            g_hash_table_remove(comp->buffers,GST_BUFFER_DATA(buf));
            g_free(info);
        }
        gst_buffer_unref(buf);
    }
}

/**
 * mpf_component_find_buffer:
 * @comp: the #MpfComponent to search, or NULL for the current component
 * @data: the data pointer to retrieve the corresponding internal GstBuffer of
 *
 * Returns: the #GstBuffer that owns the given @data, or NULL if there is no such buffer
 */
GstBuffer *
mpf_component_find_buffer (MpfComponent *comp, gpointer data) {
    MpfComponentBufferInfo *info;

    if (comp == NULL) comp = mpf_component_get_curcomponent();
    g_assert(comp != NULL);
    g_assert(MPF_IS_COMPONENT(comp));

    info = (MpfComponentBufferInfo *)g_hash_table_lookup(comp->buffers, data);
    if (info == NULL) return NULL;
    return info->buf;
}

/**
 * mpf_component_add_input_with_caps:
 * @klass: the #MpfComponentClass to add the pad to, or NULL for the current class
 * @padname: the name of the new input pad to create
 * @caps: the #GstCaps for the new pad
 *
 * Add an input pad to the component with the given name, and present a set of caps to the world as the union of all valid input formats.
 */
void
mpf_component_add_input_with_caps_presence (MpfComponentClass *klass,
        const gchar *padname, GstCaps *caps, GstPadPresence presence)
{
    GstPadTemplate *template;

    if (klass == NULL) klass = mpf_component_get_curklass();
    g_assert(klass != NULL);

    g_assert(padname != NULL);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));

    template = gst_pad_template_new(padname, GST_PAD_SINK, presence, caps);
    g_assert(template);

    klass->padtemplates = g_list_append(klass->padtemplates, template);
    klass->num_sink_pads++;
}

void
mpf_component_add_input_with_caps (MpfComponentClass *klass,
        const gchar *padname, GstCaps *caps)
{
    mpf_component_add_input_with_caps_presence(klass, padname, caps, GST_PAD_ALWAYS);
}

void mpf_component_add_input(MpfComponentClass *klass, GstPadTemplate *template) {
    if (klass == NULL) klass = mpf_component_get_curklass();
    g_assert(klass != NULL);
    g_assert(template);

    klass->padtemplates = g_list_append(klass->padtemplates, template);
    klass->num_sink_pads++;
}

gboolean
mpf_component_is_linked (MpfComponent *comp, const gchar *padname)
{
    GstPad *pad;

    if (comp == NULL) comp = mpf_component_get_curcomponent();
    g_assert(comp != NULL);           /* paranoid! */
    g_assert(MPF_IS_COMPONENT(comp)); /* paranoid! */

    pad = gst_element_get_pad(GST_ELEMENT(comp),padname);
    if (pad == NULL) {
        g_error("pad '%s' doesn't exist!",padname);
        return FALSE;
    }
    g_assert(GST_IS_PAD(pad));        /* paranoid! */
    return gst_pad_is_linked(pad);
}

/**
 * mpf_component_add_output_with_caps:
 * @klass: the #MpfComponentClass to add the pad to, or NULL for the current class
 * @padname: the name of the new output pad to create
 * @caps: the #GstCaps for the new pad
 *
 * Add an output pad to the component with the given name, and present a set of caps to the world as the union of all valid input formats.
 */
void
mpf_component_add_output_with_caps (MpfComponentClass *klass,
        const gchar *padname, GstCaps *caps)
{
    GstPadTemplate *template;

    if (klass == NULL) klass = mpf_component_get_curklass();
    g_assert(klass != NULL);

    g_assert(padname != NULL);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));

    template = gst_pad_template_new(padname, GST_PAD_SRC, GST_PAD_ALWAYS, caps);
    g_assert(template);

    klass->padtemplates = g_list_append(klass->padtemplates, template);
    klass->num_sink_pads++;
}

/**
 * mpf_component_add_input_with_capstr:
 * @klass: the #MpfComponentClass to add the pad to, or NULL for the current class
 * @padname: the name of the new input pad to create
 * @capstr: a GStreamer-style caps string
 *
 * Add an input pad to the component with the given name, and present a set of caps to the world as the union of all valid input formats.
 */
void
mpf_component_add_input_with_capstr (MpfComponentClass *klass,
        const gchar *padname, const gchar *capstr)
{
    GstPadTemplate *template;
    GstCaps *caps;

    if (klass == NULL) klass = mpf_component_get_curklass();
    g_assert(klass != NULL);

    g_assert(padname != NULL);

    g_assert(capstr != NULL);
    caps = gst_caps_from_string(capstr);
    g_assert(caps != NULL);

    template = gst_pad_template_new(padname, GST_PAD_SINK, GST_PAD_ALWAYS, caps);
    g_assert(template);

    klass->padtemplates = g_list_append(klass->padtemplates, template);
    klass->num_sink_pads++;
}

/**
 * mpf_component_add_output_with_capstr:
 * @klass: the #MpfComponentClass to add the pad to, or NULL for the current class
 * @padname: the name of the new output pad to create
 * @capstr: a GStreamer-style caps string
 *
 * Add an output pad to the component with the given name, and present a set of caps to the world as the union of all valid input formats.
 */
void
mpf_component_add_output_with_capstr (MpfComponentClass *klass,
        const gchar *padname, const gchar *capstr)
{
    GstPadTemplate *template;
    GstCaps *caps;

    if (klass == NULL) klass = mpf_component_get_curklass();
    g_assert(klass != NULL);

    g_assert(padname != NULL);

    g_assert(capstr != NULL);
    caps = gst_caps_from_string(capstr);
    g_assert(caps != NULL);

    template = gst_pad_template_new(padname, GST_PAD_SRC, GST_PAD_ALWAYS, caps);
    g_assert(template);

    klass->padtemplates = g_list_append(klass->padtemplates, template);
    klass->num_sink_pads++;
}


/**
 * mpf_component_template_get_pad:
 * @klass: the #MpfComponentClass to find the template in
 * @padname: name of the pad template to find
 *
 * Look for the given named pad in the component class and return its template.
 */
GstPadTemplate *
mpf_component_template_get_pad (MpfComponentClass *klass,const gchar *padname)
{
    GList *walk;
    GstPadTemplate *template;

    if (klass == NULL) klass = mpf_component_get_curklass();
    g_assert(klass != NULL);

    walk = klass->padtemplates;
    while (walk) {
        template = GST_PAD_TEMPLATE(walk->data);
        if (!strcmp(GST_PAD_TEMPLATE_NAME_TEMPLATE(template),padname)) {
            return template;
        }
        walk = g_list_next(walk);
    }

    return NULL;
}


/**
 * mpf_component_make_data_writable:
 * @data:
 * @rect: a valid #MpfComponentRectifyData.
 *
 */
gpointer
mpf_component_make_data_writable(gpointer data, MpfComponentRectifyData rect)
{
    MpfComponent *component = mpf_component_get_curcomponent();
    GstBuffer *buf, *newbuf;

    /* See if there's a buffer associated with this image */
    buf = mpf_component_find_buffer(component,(gpointer)data);

    /* If there is no buffer, we own it and don't need a copy */
    if (buf == NULL)
        return data;

    /* Otherwise, make a copy if we need to, and rectify */
    if (gst_buffer_is_writable(buf)) {
        return data;
    } else {
        newbuf = gst_buffer_make_writable(buf);
        rect(component, GST_BUFFER_DATA(newbuf));
        return GST_BUFFER_DATA(newbuf);
    }
}


void
mpf_component_add_caps (MpfComponentClass *klass, const gchar *padname, const gchar *caps)
{
    GstPadTemplate *padtemplate;
    GstCaps *templatecaps;
    GstCaps *newcaps;
    GstCaps *unioncaps;

    // Pick up the component klass
    if (klass == NULL)
        klass = mpf_component_get_curklass();
    g_assert(klass != NULL);
    g_assert(MPF_IS_COMPONENT_CLASS(klass));

    // Find the template
    padtemplate = mpf_component_template_get_pad(klass, padname);
    g_assert(padtemplate != NULL);
    g_assert(GST_IS_PAD_TEMPLATE(padtemplate));

    // Get its caps
    templatecaps = GST_PAD_TEMPLATE_CAPS(padtemplate);
    g_assert(templatecaps != NULL);

    // Construct the new caps from the string
    newcaps = gst_caps_from_string(caps);
    g_assert(newcaps != NULL);

    // Create the union of the template and new caps
    unioncaps = gst_caps_union(templatecaps, newcaps);
    g_assert(unioncaps);

    // Switch out the caps pointer in the template itself
    GST_PAD_TEMPLATE_CAPS(padtemplate) = unioncaps;

    // Unref the old template caps and the 'new' caps
    gst_caps_unref(templatecaps);
    gst_caps_unref(newcaps);

    return;
}



void
mpf_component_remove_caps (MpfComponentClass *klass, const gchar *padname, const gchar *caps)
{
    GstPadTemplate *padtemplate;
    GstCaps *templatecaps;
    GstCaps *newcaps;
    GstCaps *diffcaps;

    // Pick up the component klass
    if (klass == NULL)
        klass = mpf_component_get_curklass();
    g_assert(klass != NULL);
    g_assert(MPF_IS_COMPONENT_CLASS(klass));

    // Find the template
    padtemplate = mpf_component_template_get_pad(klass, padname);
    g_assert(padtemplate != NULL);
    g_assert(GST_IS_PAD_TEMPLATE(padtemplate));

    // Get its caps
    templatecaps = GST_PAD_TEMPLATE_CAPS(padtemplate);
    g_assert(templatecaps != NULL);

    // Construct the new caps from the string
    newcaps = gst_caps_from_string(caps);
    g_assert(newcaps != NULL);

    // Create the union of the template and new caps
    diffcaps = gst_caps_subtract(templatecaps, newcaps);
    g_assert(diffcaps);

    // Switch out the caps pointer in the template itself
    GST_PAD_TEMPLATE_CAPS(padtemplate) = diffcaps;

    // Unref the old template caps and the 'new' caps
    gst_caps_unref(templatecaps);
    gst_caps_unref(newcaps);

    return;
}


void
mpf_component_set_caps (MpfComponentClass *klass, const gchar *padname, const gchar *caps)
{
    GstPadTemplate *padtemplate;
    GstCaps *templatecaps;
    GstCaps *newcaps;
    GstCaps *unioncaps;

    // Pick up the component klass
    if (klass == NULL)
        klass = mpf_component_get_curklass();
    g_assert(klass != NULL);
    g_assert(MPF_IS_COMPONENT_CLASS(klass));

    // Find the template
    padtemplate = mpf_component_template_get_pad(klass, padname);
    g_assert(padtemplate != NULL);
    g_assert(GST_IS_PAD_TEMPLATE(padtemplate));

    // Get its caps
    templatecaps = GST_PAD_TEMPLATE_CAPS(padtemplate);
    g_assert(templatecaps != NULL);

    // Construct the new caps from the string
    newcaps = gst_caps_from_string(caps);
    g_assert(newcaps != NULL);

    // Create the union of the template and new caps
    unioncaps = gst_caps_union(templatecaps, newcaps);
    g_assert(unioncaps);

    // Switch out the caps pointer in the template itself
    GST_PAD_TEMPLATE_CAPS(padtemplate) = unioncaps;

    // Unref the old template caps and the 'new' caps
    gst_caps_unref(templatecaps);
    gst_caps_unref(newcaps);

    return;
}

guint64
mpf_buffer_get_offset(void *ptr) {
    MpfComponent *component;
    GstBuffer *buf;

    component = mpf_component_get_curcomponent();

    // Get the buffer
    buf = mpf_component_find_buffer(component,ptr);
    g_assert(buf != NULL);
    g_assert(GST_IS_BUFFER(buf));

    return GST_BUFFER_OFFSET(buf);
}

void
mpf_buffer_set_timestamp(void *ptr,GstClockTime time)
{
    MpfComponent *component;
    GstBuffer *buf;

    component = mpf_component_get_curcomponent();

    // Get the buffer
    buf = mpf_component_find_buffer(component,ptr);
    g_assert(buf != NULL);
    g_assert(GST_IS_BUFFER(buf));

    GST_BUFFER_TIMESTAMP(buf) = time;
}

void
mpf_buffer_set_duration(void *ptr,GstClockTime time)
{
    MpfComponent *component;
    GstBuffer *buf;

    component = mpf_component_get_curcomponent();

    // Get the buffer
    buf = mpf_component_find_buffer(component,ptr);
    g_assert(buf != NULL);
    g_assert(GST_IS_BUFFER(buf));

    GST_BUFFER_DURATION(buf) = time;
}

GstClockTime
mpf_buffer_get_timestamp(void *ptr) {
    MpfComponent *component;
    GstBuffer *buf;

    component = mpf_component_get_curcomponent();

    // Get the buffer
    buf = mpf_component_find_buffer(component,ptr);
    g_assert(buf != NULL);
    g_assert(GST_IS_BUFFER(buf));

    return GST_BUFFER_TIMESTAMP(buf);
}

GstClockTime
mpf_buffer_get_duration(void *ptr) {
    MpfComponent *component;
    GstBuffer *buf;

    component = mpf_component_get_curcomponent();

    // Get the buffer
    buf = mpf_component_find_buffer(component,ptr);
    g_assert(buf != NULL);
    g_assert(GST_IS_BUFFER(buf));

    return GST_BUFFER_DURATION(buf);
}

GstCaps *
mpf_buffer_get_caps(void *ptr)
{
    MpfComponent *component;
    GstBuffer *buf;

    component = mpf_component_get_curcomponent();

    // Get the buffer
    buf = mpf_component_find_buffer(component,ptr);
    g_assert(buf != NULL);
    g_assert(GST_IS_BUFFER(buf));

    return GST_BUFFER_CAPS(buf);
}

void
mpf_buffer_set_caps(void *ptr, GstCaps *caps)
{
    MpfComponent *component;
    GstBuffer *buf;

    component = mpf_component_get_curcomponent();

    // Get the buffer
    buf = mpf_component_find_buffer(component,ptr);
    g_assert(buf != NULL);
    g_assert(GST_IS_BUFFER(buf));

    gst_buffer_set_caps(buf, caps);
}


/**
 * mpf_buffer_get_mimetype:
 * @caps: the #GstCaps to retrieve the MIME type from
 *
 * Pull the MIME type from a #GstCaps (that presumably are fixed, not variable)
 *
 * Returns: the MIME type string
 */
const gchar *
mpf_buffer_get_mimetype(void *ptr)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);

    // If caps is NULL, then return a fake 'any' mime/type
    if (caps == NULL) {
        return "any";
    }

    // Otherwise make sure we really do have a caps struct
    g_assert(GST_IS_CAPS(caps));

    // Get the first
    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);

    return gst_structure_get_name(s);
}

/**
 * mpf_caps_get_int:
 * @caps: the #GstCaps to retrieve a value from
 * @field: the name of the field to retrieve
 *
 * Get the numeric value of a particular field (known a priori as per the MIME
 * type).
 *
 * Returns: the value of the field (assertion if field doesn't exist, FIXME?)
 */
gint
mpf_buffer_get_int(void *ptr, const gchar *field)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;
    const GValue *val;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));
    g_assert(field != NULL);

    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);
    g_assert(GST_IS_STRUCTURE(s));

    val = gst_structure_get_value(s, field);
    g_assert(val != NULL);
    g_assert(G_IS_VALUE(val));

    g_assert(G_VALUE_HOLDS_INT(val));
    return g_value_get_int(val);
}

/**
 * mpf_caps_get_int:
 * @caps: the #GstCaps to retrieve a value from
 * @field: the name of the field to retrieve
 *
 * Get the numeric value of a particular field (known a priori as per the MIME
 * type).
 *
 * Returns: the value of the field (assertion if field doesn't exist, FIXME?)
 */
gfloat
mpf_buffer_get_float(void *ptr, const gchar *field)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;
    const GValue *val;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));
    g_assert(field != NULL);

    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);
    g_assert(GST_IS_STRUCTURE(s));

    val = gst_structure_get_value(s, field);
    g_assert(val != NULL);
    g_assert(G_IS_VALUE(val));

    g_assert(G_VALUE_HOLDS_FLOAT(val));
    return g_value_get_float(val);
}

/**
 * mpf_caps_get_string:
 * @caps: the #GstCaps to retrieve a value from
 * @field: the name of the field to retrieve
 *
 * Get the string value of a particular field (known a priori as per the MIME
 * type).
 *
 * Returns: the value of the field (assertion if field doesn't exist, FIXME?)
 */
const gchar *
mpf_buffer_get_string(void *ptr, const gchar *field)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;
    const GValue *val;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));
    g_assert(field != NULL);

    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);
    g_assert(GST_IS_STRUCTURE(s));

    val = gst_structure_get_value(s, field);
    g_assert(val != NULL);
    g_assert(G_IS_VALUE(val));

    g_assert(G_VALUE_HOLDS_STRING(val));
    return g_value_get_string(val);
}



static GstStateChangeReturn
mpf_component_change_state (GstElement *element, GstStateChange transition)
{
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
    MpfComponent *component = MPF_COMPONENT (element);
    MpfComponentClass *klass = MPF_COMPONENT_CLASS(G_OBJECT_GET_CLASS(component));

    if (transition == GST_STATE_CHANGE_NULL_TO_READY) {
        if (klass->setupfunc != NULL) {
            mpf_component_set_curcomponent(component);
            klass->setupfunc(component);
        }
    } else if (transition == GST_STATE_CHANGE_READY_TO_NULL) {
        if (klass->teardownfunc != NULL) {
            mpf_component_set_curcomponent(component);
            klass->teardownfunc(component);
        }
    }

    ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
    if (ret == GST_STATE_CHANGE_FAILURE)
        return ret;

    return GST_STATE_CHANGE_SUCCESS;
}

void mpf_buffer_set_int(void *ptr, const gchar *field, gint value)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;
    GValue *val;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));
    g_assert(field != NULL);

    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);
    g_assert(GST_IS_STRUCTURE(s));

    val = (GValue*)gst_structure_get_value(s, field);
    g_assert(val != NULL);
    g_assert(G_IS_VALUE(val));

    g_value_set_int(val, value);

    g_assert(G_VALUE_HOLDS_INT(val));
}

gint
mpf_buffer_get_fraction_numerator(void *ptr, const gchar *field)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;
    const GValue *val;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));
    g_assert(field != NULL);

    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);
    g_assert(GST_IS_STRUCTURE(s));

    val = gst_structure_get_value(s, field);
    g_assert(val != NULL);
    g_assert(G_IS_VALUE(val));

    g_assert(GST_VALUE_HOLDS_FRACTION(val));
    return gst_value_get_fraction_numerator(val);
}

gint
mpf_buffer_get_fraction_denominator(void *ptr, const gchar *field)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;
    const GValue *val;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));
    g_assert(field != NULL);

    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);
    g_assert(GST_IS_STRUCTURE(s));

    val = gst_structure_get_value(s, field);
    g_assert(val != NULL);
    g_assert(G_IS_VALUE(val));

    g_assert(GST_VALUE_HOLDS_FRACTION(val));
    return gst_value_get_fraction_denominator(val);
}

void mpf_buffer_set_fraction(void *ptr, const gchar *field, gint numerator, gint denominator)
{
    GstBuffer *buf;
    GstCaps *caps;
    GstStructure *s;
    GValue *val;

    // Find the buffer
    buf = mpf_component_find_buffer(NULL,ptr);
    g_assert(buf);
    g_assert(GST_IS_BUFFER(buf));

    // Get the caps
    caps = GST_BUFFER_CAPS(buf);
    g_assert(caps != NULL);
    g_assert(GST_IS_CAPS(caps));
    g_assert(field != NULL);

    s = gst_caps_get_structure(caps, 0);
    g_assert(s != NULL);
    g_assert(GST_IS_STRUCTURE(s));

    val = (GValue*)gst_structure_get_value(s, field);
    g_assert(val != NULL);
    g_assert(G_IS_VALUE(val));

    gst_value_set_fraction(val, numerator, denominator);

    g_assert(GST_VALUE_HOLDS_FRACTION(val));
}

