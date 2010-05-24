SUBDIRS = mpf components

EXTRA_DIST =				\
	autogen.sh			\
	gst-autogen.sh			\
	mpf-newdatatype.spec		\
	mpf-newdatatype.spec.in		\
	mpf-newdatatype.pc.in		\
	mpf-newdatatype-uninstalled.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mpf-newdatatype.pc

svnclean:
	make distclean
	rm -rf configure Makefile.in depcomp config.guess \
		ltmain.sh config.sub config.h.in autom4te.cache \
		missing aclocal.m4 install-sh INSTALL \
		$(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*

