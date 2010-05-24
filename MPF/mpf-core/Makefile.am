SUBDIRS = mpf tools docs

DIST_SUBDIRS = $(SUBDIRS) test

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

EXTRA_DIST = autogen.sh gst-autogen.sh mpf-core.spec.in

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mpf-core.pc

svnclean:
	make distclean
	pushd m4; make distclean; popd
	rm -rf configure Makefile.in tools/Makefile.in depcomp config.guess \
	    ltmain.sh config.sub config.h.in autom4te.cache mpf-voidstar.spec\
	    missing aclocal.m4 install-sh INSTALL \
	    $(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
