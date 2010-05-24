SUBDIRS = src test

EXTRA_DIST =					\
	autogen.sh				\
	grdf.spec				\
	grdf.spec.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = grdf.pc

svnclean:
	make distclean
	rm -rf configure Makefile.in depcomp config.guess \
		ltmain.sh config.sub config.h.in autom4te.cache \
		missing aclocal.m4 install-sh INSTALL \
		$(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
