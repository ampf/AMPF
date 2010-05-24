/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <xml.h>

#include <mpf/mpf-component.h>

#include "mpf-scenedata.h"


static GstStaticCaps mpf_scenedata_caps =
  GST_STATIC_CAPS ("application/x-mpf-scenedata");


void
mpf_scenedata_add_input (const gchar *name)
{
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;

  template = gst_pad_template_new(name, GST_PAD_SINK, GST_PAD_ALWAYS,
		gst_static_caps_get(&mpf_scenedata_caps));

  klass->padtemplates = g_list_append(klass->padtemplates, template);
}

void
mpf_scenedata_add_output (const gchar *name)
{
  MpfComponentClass *klass = mpf_component_get_curklass();
  GstPadTemplate *template;

  template = gst_pad_template_new(name, GST_PAD_SRC, GST_PAD_ALWAYS,
		gst_static_caps_get(&mpf_scenedata_caps));

  klass->padtemplates = g_list_append(klass->padtemplates, template);
}

xmlDocPtr
mpf_pad_pull_scenedata (const gchar *padname)
{
  MpfComponent *component = mpf_component_get_curcomponent();
  GstPad *pad;
  MpfComponentPadInfo *padinfo;
  GstBuffer *buf;
  xmlDocPtr doc;

  pad = gst_element_get_pad(GST_ELEMENT(component),padname);
  padinfo = (MpfComponentPadInfo *)gst_pad_get_element_private(pad);
  buf = (GstBuffer *)(g_list_first(padinfo->buffers)->data);
  padinfo->buffers = g_list_remove(padinfo->buffers, buf);

  g_hash_table_insert(component->bufferMap,iplimage,buf);

