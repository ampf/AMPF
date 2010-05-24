# Define if you want to place your source in a subdirectory, and move the rest of 
# this file into that directory's Makefile.am.  You must also reference that 
# Makefile.am at the end of configure.ac in the AC_OUTPUT block.
#SUBDIRS = src

plugin_LTLIBRARIES = libmpf-shift.la

libmpf_shift_la_SOURCES = mpf-shift.c

EXTRA_DIST =					\
	autogen.sh				\
	mpf-shift.spec		\
	mpf-shift.spec.in

svnclean:
	make distclean
	rm -rf configure Makefile.in depcomp config.guess \
		ltmain.sh config.sub config.h.in autom4te.cache \
		missing aclocal.m4 install-sh INSTALL \
		$(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
