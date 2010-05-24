lib_LTLIBRARIES = libmpf-utterance.la

libmpf_utterance_la_SOURCES = mpf-utterance.c
libmpf_utterance_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS)
libmpf_utterance_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS)

libmpf_utteranceincludedir = $(includedir)/gstreamer/mpf
libmpf_utteranceinclude_HEADERS = mpf-utterance.h

plugin_LTLIBRARIES = libmpfpocketsphinx.la libutterance-dump.la libutterance-filter.la

libmpfpocketsphinx_la_SOURCES = mpfpocketsphinx.c psmarshal.c
libmpfpocketsphinx_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(POCKETSPHINX_CFLAGS) -DMODELDIR=\"$(POCKETSPHINX_MODELDIR)\"
libmpfpocketsphinx_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(POCKETSPHINX_LIBS)
libmpfpocketsphinx_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

noinst_HEADERS = mpfpocketsphinx.h gstvader.h psmarshal.h

libutterance_dump_la_SOURCES = utterance-dump.c
libutterance_dump_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS)
libutterance_dump_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) libmpf-utterance.la
libutterance_dump_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libutterance_filter_la_SOURCES = utterance-filter.c
libutterance_filter_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS)
libutterance_filter_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) libmpf-utterance.la
libutterance_filter_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

EXTRA_DIST = test.sh
