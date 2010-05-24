# Define if you want to place your source in a subdirectory, and move the rest of 
# this file into that directory's Makefile.am.  You must also reference that 
# Makefile.am at the end of configure.ac in the AC_OUTPUT block.
#SUBDIRS = src

bin_PROGRAMS = test-1-pipe test-2-pipe

test_1_pipe_SOURCES = test-1-pipe.c
test_2_pipe_SOURCES = test-2-pipe.c

plugin_LTLIBRARIES = libmpf-test-pipe.la

libmpf_test_pipe_la_SOURCES = mpf-test-pipe.c

EXTRA_DIST =					\
	autogen.sh				\
	mpf-test-pipe.spec		\
	mpf-test-pipe.spec.in

svnclean:
	make distclean
	rm -rf configure Makefile.in depcomp config.guess \
		ltmain.sh config.sub config.h.in autom4te.cache \
		missing aclocal.m4 install-sh INSTALL \
		$(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*
