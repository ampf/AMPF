/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
/* -*- c-basic-offset: 4; indent-tabs-mode: nil -*- */
/* ====================================================================
 * Copyright (c) 2007 Carnegie Mellon University.  All rights
 * reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * This work was supported in part by funding from the Defense Advanced
 * Research Projects Agency and the National Science Foundation of the
 * United States of America, and the CMU Sphinx Speech Consortium.
 *
 * THIS SOFTWARE IS PROVIDED BY CARNEGIE MELLON UNIVERSITY ``AS IS'' AND
 * ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL CARNEGIE MELLON UNIVERSITY
 * NOR ITS EMPLOYEES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ====================================================================
 *
 * Author: David Huggins-Daines <dhuggins@cs.cmu.edu>
 * Contributors:
 *    Appscio, Inc - Erik Walthinsen <ewalthinsen@appscio.com> map into MPF
 *
 */

#ifndef __MPF_POCKETSPHINX_H__
#define __MPF_POCKETSPHINX_H__

#include <gst/gst.h>
#include <sphinx_config.h>
#include <pocketsphinx.h>

G_BEGIN_DECLS
/* --- type macros --- */
/**
 * MPF_TYPE_POCKETSPHINX:
 *
 * The type for #MpfPocketSphinx.
 */
#define MPF_TYPE_POCKETSPHINX                   \
    (mpf_pocketsphinx_get_type())
/**
 * MPF_POCKETSPHINX:
 * @obj: Object which is subject to casting.
 *
 * Casts a #MpfPocketSphinx or derived pointer into a (MpfPocketSphinx*) pointer.
 * Depending on the current debugging level, this function may invoke certain runtime
 * checks to identify invalid casts.
 */
#define MPF_POCKETSPHINX(obj)                                           \
    (G_TYPE_CHECK_INSTANCE_CAST((obj),MPF_TYPE_POCKETSPHINX,MpfPocketSphinx))
/**
 * MPF_POCKETSPHINX_CLASS:
 * @klass: a valid #MpfPocketSphinxClass
 *
 * Casts a derived #MpfPocketSphinxClass structure into a #MpfPocketSphinxClass structure.
 */
#define MPF_POCKETSPHINX_CLASS(klass)                                   \
    (G_TYPE_CHECK_CLASS_CAST((klass),MPF_TYPE_POCKETSPHINX,MpfPocketSphinxClass))
/**
 * MPF_IS_POCKETSPHINX:
 * @obj: Instance to check for being a %MPF_TYPE_POCKETSPHINX.
 *
 * Checks whether a valid #GTypeInstance pointer is of type %MPF_TYPE_POCKETSPHINX.
 */
#define MPF_IS_POCKETSPHINX(obj)                                \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj),MPF_TYPE_POCKETSPHINX))
/**
 * MPF_IS_POCKETSPHINX_CLASS:
 * @klass: a #MpfPocketSphinxClass
 *
 * Checks whether @klass "is a" valid #MpfPocketSphinxClass structure of type
 * %MPF_TYPE_POCKETSPHINX or derived.
 */
#define MPF_IS_POCKETSPHINX_CLASS(klass)                        \
    (G_TYPE_CHECK_CLASS_TYPE((klass),MPF_TYPE_POCKETSPHINX))

/* --- typedefs & structures --- */
typedef struct _MpfPocketSphinx      MpfPocketSphinx;
typedef struct _MpfPocketSphinxClass MpfPocketSphinxClass;

/**
 * MpfPocketSphinx:
 *
 * All the fields in the <structname>MpfPocketSphinx</structname> structure are private
 * to the #MpfPocketSphinx implementation and should never be accessed directly.
 */
struct _MpfPocketSphinx
{
	/*< private >*/
    GstElement element;

    GstPad *sinkpad, *srcpad;

    ps_decoder_t *ps;
    cmd_ln_t *config;
    gchar *latdir;                 /*< Output directory for word lattices. >*/

    GHashTable *arghash;
    gboolean listening;

    GstClockTime last_result_time; /*< Timestamp of last partial result. >*/
    char *last_result;             /*< String of last partial result. >*/
};

/**
 * MpfPocketSphinxClass:
 *
 * All the fields in the <structname>MpfPocketSphinxClass</structname> structure are private
 * to the #MpfPocketSphinxClass implementation and should never be accessed directly.
 */
struct _MpfPocketSphinxClass
{
	/*< private >*/
    GstElementClass parent_class;

    void (*partial_result)  (GstElement *element, const gchar *hyp_str);
    void (*result)          (GstElement *element, const gchar *hyp_str);
};

/*
 * Boxing of lattices.
 */
#define PS_LATTICE_TYPE (ps_lattice_get_type())
GType ps_lattice_get_type(void);

/*
 * Boxing of decoder.
 */
#define PS_DECODER_TYPE (ps_decoder_get_type())
GType ps_decoder_get_type(void);

/* --- prototypes --- */
GType gst_pocketsphinx_get_type(void);


G_END_DECLS

#endif /* __MPF_POCKETSPHINX_H__ */
