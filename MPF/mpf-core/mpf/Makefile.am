# plugindir is set in configure

lib_LTLIBRARIES = libmpf-core.la

libmpf_core_la_SOURCES =	\
	mpf-component.c		\
	mpf-voidstar.c		\
	mpf-compound-buffer.c	\
	mpf-rawvideo.c \
	mpf-logging.c

libmpf_core_la_CFLAGS = $(GST_CFLAGS) $(GST_VIDEO_CFLAGS) -Werror -Wall
libmpf_core_la_LIBADD = $(GST_LIBS) $(GST_VIDEO_LIBS)

libmpf_core_includedir = $(includedir)/mpf
libmpf_core_include_HEADERS =		\
	mpf-component.h			\
	mpf-component-magicfuncs.h	\
	mpf-componenttemplate.h		\
	mpf-voidstar.h			\
	mpf-compound-buffer.h		\
	mpf-rawvideo.h \
	mpf-logging.h
