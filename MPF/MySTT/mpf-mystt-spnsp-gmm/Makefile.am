# Define if you want to place your source in a subdirectory, and move the rest of 
# this file into that directory's Makefile.am.  You must also reference that 
# Makefile.am at the end of configure.ac in the AC_OUTPUT block.
#SUBDIRS = src

plugin_LTLIBRARIES = libmpf-mystt-spnsp-gmm.la

libmpf_mystt_spnsp_gmm_la_SOURCES = mpf-mystt-spnsp-gmm.c global.cpp gmm.cpp gmmbridge.cpp global.h gmm.h gmmbridge.h speech.gmm nonspeech.gmm

EXTRA_DIST =					\
	autogen.sh				\
	mpf-mystt-spnsp-gmm.spec		\
	mpf-mystt-spnsp-gmm.spec.in

svnclean:
	make distclean
	rm -rf configure Makefile.in depcomp config.guess \
		ltmain.sh config.sub config.h.in autom4te.cache \
		missing aclocal.m4 install-sh INSTALL \
		$(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*

install-models:
	install -d $(DESTDIR)/usr/share/mpf/mystt
	cp $(top_srcdir)/speech.gmm $(DESTDIR)/usr/share/mpf/mystt/
	cp $(top_srcdir)/nonspeech.gmm $(DESTDIR)/usr/share/mpf/mystt/

install: install-models
	install -d $(DESTDIR)$(libdir)/gstreamer-0.10
	cp /usr/src/redhat/BUILD/$(PACKAGE)-$(VERSION)/.libs/libmpf-mystt-spnsp-gmm.so $(DESTDIR)$(libdir)/gstreamer-0.10/

uninstall:
	rm -f $(libdir)gstreamer-0.10/libmpf-mystt-spnsp-gmm.so
	rm -f /usr/share/mpf/mystt/speech.gmm
	rm -f /usr/share/mpf/mystt/nonspeech.gmm

