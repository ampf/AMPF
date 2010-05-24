# Define if you want to place your source in a subdirectory
# SUBDIRS = src

# Define if your sources are in the root
plugin_LTLIBRARIES =			\
	libmpf-opencv-haar.la		\
	libmpf-rdf-roimarkup.la

libmpf_opencv_haar_la_SOURCES = mpf-opencv-haar.c
libmpf_rdf_roimarkup_la_SOURCES = mpf-rdf-roimarkup.c

EXTRA_DIST =					\
	autogen.sh				\
	mpf-rdftest.spec			\
	mpf-rdftest.spec.in
	
svnclean:
	make distclean
	rm -rf configure Makefile.in depcomp config.guess \
		ltmain.sh config.sub config.h.in autom4te.cache \
		missing aclocal.m4 install-sh INSTALL \
		$(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*

