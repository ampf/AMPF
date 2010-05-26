SUBDIRS = mpf tools docs

DIST_SUBDIRS = $(SUBDIRS) test

DISTCHECK_CONFIGURE_FLAGS=--enable-gtk-doc

EXTRA_DIST = autogen.sh gst-autogen.sh mpf-core.spec.in

ACLOCAL_AMFLAGS = -I m4

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = mpf-core.pc

svnclean:
	-(pushd m4; make distclean; popd)
	-(pushd mpf; make svnclean; popd)
	-(pushd tools; make svnclean; popd)
	-(pushd docs; make distclean; popd)
	-make distclean
	-rm -rf configure Makefile.in tools/Makefile.in depcomp config.guess \
	    ltmain.sh config.sub config.h.in autom4te.cache mpf-voidstar.spec\
	    missing aclocal.m4 install-sh INSTALL autoregen.sh \
	    $(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
