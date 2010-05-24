plugin_LTLIBRARIES =				\
        libtoiplimage.la			\
        libfromiplimage.la

libtoiplimage_la_SOURCES = toiplimage.c
libtoiplimage_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS) -I..
libtoiplimage_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) ../mpf/libmpf-iplimage.la
libtoiplimage_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libfromiplimage_la_SOURCES = fromiplimage.c
libfromiplimage_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS) -I..
libfromiplimage_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) ../mpf/libmpf-iplimage.la
libfromiplimage_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

noinst_HEADERS = toiplimage.h fromiplimage.h

