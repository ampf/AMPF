/*******************************************************************************
 * Copyright (c) 2010 Appscio Dual License. All rights reserved. This program and the accompanying materials are made available under your choice of the GNU Lesser Public License v2.1 or the Apache License, Version 2.0. See the included License-dual file.
 *******************************************************************************/
#define component_class_init(...)									\
static void _component_class_init (MpfComponentClass *klass);						\
static void __component_class_init_set_ptr() __attribute__ ((constructor));				\
static void __component_class_init_set_ptr() { __component_class_init = _component_class_init; }	\
static void _component_class_init (MpfComponentClass *klass)

typedef void (*__component_class_init_funcptr) (MpfComponentClass *klass);
#ifndef MPF_DISABLE_UNUSED
static __component_class_init_funcptr __component_class_init __attribute__ ((unused));	// FIXME
#endif
static __component_class_init_funcptr __component_class_init = NULL;


#define component_class_finalize(...)										\
static void _component_class_finalize (MpfComponentClass *klass);						\
static void __component_class_finalize_set_ptr() __attribute__ ((constructor));					\
static void __component_class_finalize_set_ptr() { __component_class_finalize = _component_class_finalize; }	\
static void _component_class_finalize (MpfComponentClass *klass)

typedef void (*__component_class_finalize_funcptr) (MpfComponentClass *klass);
#ifndef MPF_DISABLE_UNUSED
static __component_class_finalize_funcptr __component_class_finalize __attribute__ ((unused));	// FIXME
#endif
static __component_class_finalize_funcptr __component_class_finalize = NULL;


#define component_init(...)								\
static void _component_init (MpfComponent *component);					\
static void __component_init_set_ptr() __attribute__ ((constructor));			\
static void __component_init_set_ptr() { __component_init = _component_init; }		\
static void _component_init (MpfComponent *component)

typedef void (*__component_init_funcptr) (MpfComponent *component);
#ifndef MPF_DISABLE_UNUSED
static __component_init_funcptr __component_init __attribute__ ((unused));	// FIXME
#endif
static __component_init_funcptr __component_init = NULL;


#define component_setup(...)								\
static void _component_setup (MpfComponent *component);					\
static void __component_setup_set_ptr() __attribute__ ((constructor));			\
static void __component_setup_set_ptr() { __component_setup = _component_setup; }	\
static void _component_setup (MpfComponent *component)

typedef void (*__component_setup_funcptr) (MpfComponent *component);
#ifndef MPF_DISABLE_UNUSED
static __component_setup_funcptr __component_setup __attribute__ ((unused));	// FIXME
#endif
static __component_setup_funcptr __component_setup = NULL;


#define component_teardown(...)									\
static void _component_teardown (MpfComponent *component);					\
static void __component_teardown_set_ptr() __attribute__ ((constructor));			\
static void __component_teardown_set_ptr() { __component_teardown = _component_teardown; }	\
static void _component_teardown (MpfComponent *component)

typedef void (*__component_teardown_funcptr) (MpfComponent *component);
#ifndef MPF_DISABLE_UNUSED
static __component_teardown_funcptr __component_teardown __attribute__ ((unused));	// FIXME
#endif
static __component_teardown_funcptr __component_teardown = NULL;


#define component_finalize(...)									\
static void _component_finalize (MpfComponent *component);					\
static void __component_finalize_set_ptr() __attribute__ ((constructor));			\
static void __component_finalize_set_ptr() { __component_finalize = _component_finalize; }	\
static void _component_finalize (MpfComponent *component)

typedef void (*__component_finalize_funcptr) (MpfComponent *component);
#ifndef MPF_DISABLE_UNUSED
static __component_finalize_funcptr __component_finalize __attribute__ ((unused));	// FIXME
#endif
static __component_finalize_funcptr __component_finalize = NULL;


#define component_process(...)								\
static MpfComponentProcessReturn _component_process (MpfComponent *component);		\
static void __component_process_set_ptr() __attribute__ ((constructor));		\
static void __component_process_set_ptr() { __component_process = _component_process; }	\
static MpfComponentProcessReturn _component_process (MpfComponent *component)

typedef MpfComponentProcessReturn (*__component_process_funcptr) (MpfComponent *component);
#ifndef MPF_DISABLE_UNUSED
static __component_process_funcptr __component_process __attribute__ ((unused));	// FIXME
#endif
static __component_process_funcptr __component_process = NULL;


#define component_set_parameter(...)										\
static void _component_set_parameter (MpfComponent *component,const gchar *paramname);				\
static void __component_set_parameter_set_ptr() __attribute__ ((constructor));					\
static void __component_set_parameter_set_ptr() { __component_set_parameter = _component_set_parameter; }	\
static void _component_set_parameter (MpfComponent *component,const gchar *paramname)

typedef void (*__component_set_parameter_funcptr) (MpfComponent *component,const gchar *paramname);
#ifndef MPF_DISABLE_UNUSED
static __component_set_parameter_funcptr __component_set_parameter __attribute__ ((unused));	// FIXME
#endif
static __component_set_parameter_funcptr __component_set_parameter = NULL;


#define component_get_parameter(...)										\
static void _component_get_parameter (MpfComponent *component,const gchar *paramname);				\
static void __component_get_parameter_set_ptr() __attribute__ ((constructor));					\
static void __component_get_parameter_set_ptr() { __component_get_parameter = _component_get_parameter; }	\
static void _component_get_parameter (MpfComponent *component,const gchar *paramname)

typedef void (*__component_get_parameter_funcptr) (MpfComponent *component,const gchar *paramname);
#ifndef MPF_DISABLE_UNUSED
static __component_get_parameter_funcptr __component_get_parameter __attribute__ ((unused));	// FIXME
#endif
static __component_get_parameter_funcptr __component_get_parameter = NULL;


