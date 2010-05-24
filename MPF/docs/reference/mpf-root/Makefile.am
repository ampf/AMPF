include $(top_srcdir)/Makefile.decl

AUTOMAKE_OPTIONS = 1.7

SUBDIRS = docs

EXTRA_DIST =      \
	autogen.sh

BUILD_EXTRA_DIST =     \
	README         \
	INSTALL        \
	gtk-doc.make

DISTCHECK_CONFIGURE_FLAGS = --enable-debug --enable-gtk-doc --enable-man
