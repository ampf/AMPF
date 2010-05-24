EXTRA_DIST =					\
	autogen.sh				\
	mpf-template.spec			\
	mpf-template.spec.in

SUBDIRS = templates tools

svnclean:
	make -C tools svnclean
	make -C templates svnclean
		make distclean
	rm -rf configure Makefile.in depcomp config.guess \
	  ltmain.sh config.sub config.h.in autom4te.cache \
	  aclocal.m4 install-sh INSTALL \
	  $(PACKAGE)-[.0-9]* $(PACKAGE)-[.0-9]*.tar*	  

.PHONY: svnclean
