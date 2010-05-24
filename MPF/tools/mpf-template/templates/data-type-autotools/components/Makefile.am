plugin_LTLIBRARIES =				\
        libtonewdatatype.la			\
        libfromnewdatatype.la

libtonewdatatype_la_SOURCES = tonewdatatype.c
libtonewdatatype_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(NEWDATATYPE_CFLAGS) -I..
libtonewdatatype_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(NEWDATATYPE_LIBS) ../mpf/libmpf-newdatatype.la
libtonewdatatype_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libfromnewdatatype_la_SOURCES = fromnewdatatype.c
libfromnewdatatype_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(NEWDATATYPE_CFLAGS) -I..
libfromnewdatatype_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(NEWDATATYPE_LIBS) ../mpf/libmpf-newdatatype.la
libfromnewdatatype_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

noinst_HEADERS = tonewdatatype.h fromnewdatatype.h

