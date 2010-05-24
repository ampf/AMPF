plugin_LTLIBRARIES = libvoidstar-sink.la libparams1.la

libvoidstar_sink_la_SOURCES = voidstar-sink.c
libvoidstar_sink_la_CFLAGS = $(GST_CFLAGS) -I..
libvoidstar_sink_la_LIBADD = $(GST_LIBS) ../mpf/libmpf-core.la
libvoidstar_sink_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libparams1_la_SOURCES = params1.c
libparams1_la_CFLAGS = $(GST_CFLAGS) -I..
libparams1_la_LIBADD = $(GST_LIBS) ../mpf/libmpf-core.la
libparams1_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
