SUBDIRS = mpf components

EXTRA_DIST =				\
	autogen.sh			\
	gst-autogen.sh			\
	mpf-iplimage.spec		\
	mpf-iplimage.spec.in		\
	mpf-iplimage.pc.in		\
	mpf-iplimage-uninstalled.pc.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mpf-iplimage.pc

svnclean:
	make distclean
	rm -rf autoregen.sh configure Makefile.in mpf/Makefile.in components/Makefile.in depcomp config.guess \
	    ltmain.sh config.sub config.h.in autom4te.cache mpf-voidstar.spec\
	    missing aclocal.m4 install-sh INSTALL \
	    $(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
