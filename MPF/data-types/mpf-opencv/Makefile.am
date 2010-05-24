SUBDIRS = mpf

EXTRA_DIST = autogen.sh gst-autogen.sh mpf-opencv.spec mpf-opencv.spec.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mpf-opencv.pc

svnclean:
	make distclean
	rm -rf configure Makefile.in mpf/Makefile.in depcomp config.guess \
	    ltmain.sh config.sub config.h.in autom4te.cache mpf-voidstar.spec\
	    missing aclocal.m4 install-sh INSTALL \
	    $(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
