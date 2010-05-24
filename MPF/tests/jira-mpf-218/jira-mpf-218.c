/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* GStreamer
 * Copyright (C) 2007 David Schleef <ds@schleef.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <mpf/mpf-component.h>

typedef struct _ElementClass {
    MpfComponentClass parent_class;
} ElementClass;

typedef struct _Element {
    MpfComponent element;
} Element;

static void element_init(Element *object, ElementClass *klass) {
    printf("jira-mpf-218: element_init\n");
}

static void element_class_init(ElementClass *klass, gpointer class_data) {
    printf("jira-mpf-218: element_class_init: class_data=%s\n", class_data);
    GObjectClass *gobject_class = (GObjectClass *) klass;
    GstElementDetails element_details = { class_data,
            "element type", "element desc",
            "element auth" };

    gst_element_class_set_details(GST_ELEMENT_CLASS(klass), &element_details);

}

static GType element_type(const gchar* name, const gchar* type) {

    GType template_type = 0;

    printf("jira-mpf-218: element_type: name=%s, type-%s\n", name, type);

    const GTypeInfo template_info = {
            sizeof(ElementClass),
            NULL,
            NULL,
            (GClassInitFunc)element_class_init,
            NULL, // (GClassFinalizeFunc) template_class_finalize,
            type,
            sizeof(Element),
            0,
            (GInstanceInitFunc)element_init,
            };

    template_type = g_type_register_static(MPF_TYPE_COMPONENT, name,
            &template_info, (GTypeFlags) 0);
    return template_type;
}

static gboolean plugin_init(GstPlugin * plugin) {

    gst_element_register(plugin, "element-1", GST_RANK_NONE, element_type(
            "element-1-pipeline", "type-1"));
    gst_element_register(plugin, "element-2", GST_RANK_NONE, element_type(
            "element-2-pipeline", "type-2"));
    return TRUE;
}

GST_PLUGIN_DEFINE (GST_VERSION_MAJOR,
        GST_VERSION_MINOR,
        "jira-mpf-218",
        "Container",
        plugin_init, VERSION, "LGPL", "mpf-container", "http://www.appscio.org")
