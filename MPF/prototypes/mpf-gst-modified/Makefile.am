
plugin_LTLIBRARIES = libmpf_multifile.la

libmpf_multifile_la_SOURCES = mpf-multifilesink.c mpf-multifile.c
libmpf_multifile_la_CFLAGS = $(GST_BASE_CFLAGS) $(GST_CFLAGS)
libmpf_multifile_la_LIBADD = $(GST_BASE_LIBS) $(GST_LIBS)
libmpf_multifile_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libmpf_multifile_la_LIBTOOLFLAGS = --tag=disable-static

noinst_HEADERS = mpf-multifilesink.h

svnclean:
	make distclean
	rm -rf configure Makefile.in depcomp config.guess \
		ltmain.sh config.sub config.h.in autom4te.cache \
		missing aclocal.m4 install-sh INSTALL \
		$(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
