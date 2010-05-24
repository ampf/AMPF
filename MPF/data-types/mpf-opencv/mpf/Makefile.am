lib_LTLIBRARIES = libmpf-iplimage.la libmpf-objlist.la

libmpf_iplimage_la_SOURCES = mpf-iplimage.c
libmpf_iplimage_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libmpf_iplimage_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS)
#libmpf_iplimage_la_LDFLAGS = $(GST_LIB_LDFLAGS)

libmpf_iplimageincludedir = $(includedir)/gstreamer/mpf
libmpf_iplimageinclude_HEADERS = mpf-iplimage.h

libmpf_objlist_la_SOURCES = mpf-objlist.c
libmpf_objlist_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS)
libmpf_objlist_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS)
#libmpf_objlist_la_LDFLAGS = $(GST_LIB_LDFLAGS)

libmpf_objlistincludedir = $(includedir)/gstreamer/mpf
libmpf_objlistinclude_HEADERS = mpf-objlist.h

plugin_LTLIBRARIES =				\
	libcvsmooth.la				\
	libcvsobel.la				\
	libcvcanny.la				\
	libcvcvtcolor.la			\
	libcvhaar.la				\
	libobjmarkup.la				\
	libobjcorrelate.la			\
	libiplsink.la				\
	libiplidentity.la

libcvsmooth_la_SOURCES = cvsmooth.c
libcvsmooth_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libcvsmooth_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la
libcvsmooth_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libcvsobel_la_SOURCES = cvsobel.c
libcvsobel_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libcvsobel_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la
libcvsobel_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libcvcanny_la_SOURCES = cvcanny.c
libcvcanny_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libcvcanny_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la
libcvcanny_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libcvcvtcolor_la_SOURCES = cvcvtcolor.c
libcvcvtcolor_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libcvcvtcolor_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la
libcvcvtcolor_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libcvhaar_la_SOURCES = cvhaar.c
libcvhaar_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libcvhaar_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la libmpf-objlist.la
libcvhaar_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libobjmarkup_la_SOURCES = objmarkup.c
libobjmarkup_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libobjmarkup_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la libmpf-objlist.la
libobjmarkup_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libobjcorrelate_la_SOURCES = objcorrelate.c
libobjcorrelate_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libobjcorrelate_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la libmpf-objlist.la
libobjcorrelate_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libiplsink_la_SOURCES = iplsink.c
libiplsink_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libiplsink_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la
libiplsink_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

libiplidentity_la_SOURCES = iplidentity.c
libiplidentity_la_CFLAGS = $(GST_CFLAGS) $(MPF_CORE_CFLAGS) $(OPENCV_CFLAGS)
libiplidentity_la_LIBADD = $(GST_LIBS) $(MPF_CORE_LIBS) $(OPENCV_LIBS) libmpf-iplimage.la
libiplidentity_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)

