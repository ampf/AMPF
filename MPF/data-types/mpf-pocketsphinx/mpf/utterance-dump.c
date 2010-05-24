/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#include <mpf/mpf-utterance.h>

#define COMPONENT_NAME "utterance-dump"
#define COMPONENT_DESC "Dump utterance info to stdout"
#define COMPONENT_AUTH "Erik Walthinsen <omega@appscio.com>"

struct component_private {
};

#include <mpf/mpf-componenttemplate.h>

static void component_class_init(MpfComponentClass *klass) {
  mpf_utterance_add_input("input");
}

static void component_init(MpfComponent *component) {
}

static MpfComponentProcessReturn component_process(MpfComponent *component) {
  MpfUtterance *utterance;

  utterance = mpf_utterance_pull("input");

  if (utterance->final) {
    fprintf(stderr,"\033[00;32mUtterance ID %d: '%s' (%d)\033[00m\n",utterance->id,utterance->text,utterance->score);
  } else {
    fprintf(stderr,"\033[00;31mUtterance ID %d: '%s' (%d)\033[00m\n",utterance->id,utterance->text,utterance->score);
  }

  mpf_utterance_unref(utterance);

  return MPF_GOOD;
}
