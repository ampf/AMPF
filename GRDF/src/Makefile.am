lib_LTLIBRARIES = libgrdf.la

libgrdf_la_SOURCES =		\
	grdf.c			\
	grdf-world.c		\
	grdf-ontology.c		\
	grdf-graph.c		\
	grdf-node.c		\
	grdf-node-literal.c	\
	grdf-node-uri.c		\
	grdf-node-anon.c	\
	grdf-node-variable.c	\
	grdf-stmt.c		
#	grdf-node-match.c
#	grdf-query.c

libgrdf_includedir = $(includedir)
libgrdf_include_HEADERS =	\
	grdf.h			\
	grdf-world.h		\
	grdf-ontology.h		\
	grdf-graph.h		\
	grdf-node.h		\
	grdf-node-literal.h	\
	grdf-node-uri.h		\
	grdf-node-anon.h	\
	grdf-node-variable.h	\
	grdf-stmt.h		
#	grdf-node-match.h
#	grdf-query.h

libgrdf_la_CFLAGS = $(CFLAGS) $(GOBJECT_CFLAGS) -g -Werror -Wall
libgrdf_la_LIBS = $(LIBADD) $(GOBJECT_LIBS)
