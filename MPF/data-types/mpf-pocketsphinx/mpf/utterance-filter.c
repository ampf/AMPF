/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-utterance.h>

#define COMPONENT_NAME "utterance-filter"
#define COMPONENT_DESC "Filter utterances to only pass 'final'"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpf-componenttemplate.h>

static void component_class_init(MpfComponentClass *klass) {
  mpf_utterance_add_input("input");
  mpf_utterance_add_output("ouptut");
}

static void component_init(MpfComponent *component) {
}

static MpfComponentProcessReturn component_process(MpfComponent *component) {
  MpfUtterance *utterance;

  utterance = mpf_utterance_pull("input");

  if (utterance->final)
    mpf_utterance_push("output",utterance);
  else
    mpf_utterance_unref(utterance);

  return MPF_GOOD;
}
