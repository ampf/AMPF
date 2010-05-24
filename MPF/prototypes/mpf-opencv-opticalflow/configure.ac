AC_INIT

PACKAGE="mpf-opencv-opticalflow"
VERSION_MAJOR=0
VERSION_MINOR=0
VERSION_MICRO=3
VERSION_NANO=0

dnl Minimum required version of various packages
GST_MAJORMINOR=0.10
GST_REQUIRED=0.10.0
MPF_CORE_REQUIRED=0.0.3

VERSION=$VERSION_MAJOR.$VERSION_MINOR.$VERSION_MICRO.$VERSION_NANO
AC_SUBST(PACKAGE)
AC_SUBST(VERSION_MAJOR)
AC_SUBST(VERSION_MINOR)
AC_SUBST(VERSION_MICRO)
AC_SUBST(VERSION_NANO)
AC_SUBST(VERSION)

dnl AM_MAINTAINER_MODE provides the option to enable maintainer mode
AM_MAINTAINER_MODE

dnl Start up automake
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AM_CONFIG_HEADER(config.h)

dnl check for tools
AC_PROG_CC
AC_PROG_LIBTOOL


dnl Check for pkgconfig first
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, yes, no)

dnl Give error and exit if we don't have pkgconfig
if test "x$HAVE_PKGCONFIG" = "xno"; then
  AC_MSG_ERROR(you need to have pkgconfig installed !)
fi


dnl ***** This is a development-time tool to get all the -uninstalled.pc paths
m4_pattern_allow([PKG_CONFIG_PATH_CONSTRUCTOR])
if [[ "x$PKG_CONFIG_PATH_CONSTRUCTOR" != "x" ]];then
  export PKG_CONFIG_PATH=$PKG_CONFIG_PATH:`$PKG_CONFIG_PATH_CONSTRUCTOR`
fi



PKG_CHECK_MODULES(MPF_CORE, mpf-core,
  ,  AC_MSG_ERROR(no MPF Core libraries found (mpf-core)))

dnl make _CFLAGS and _LIBS available
AC_SUBST(MPF_CORE_CFLAGS)
AC_SUBST(MPF_CORE_LIBS)


PKG_CHECK_MODULES(MPF_IPLIMAGE, mpf-iplimage,
  ,  AC_MSG_ERROR(no MPF IplImage libraries found (mpf-iplimage)))

dnl make _CFLAGS and _LIBS available
AC_SUBST(MPF_IPLIMAGE_CFLAGS)
AC_SUBST(MPF_IPLIMAGE_LIBS)


PKG_CHECK_MODULES(MPF_RDF, mpf-rdf,
  ,  AC_MSG_ERROR(no MPF RDF libraries found (mpf-rdf)))

dnl make _CFLAGS and _LIBS available
AC_SUBST(MPF_RDF_CFLAGS)
AC_SUBST(MPF_RDF_LIBS)


dnl set the plugindir where plugins should be installed
if test "x${prefix}" = "x$HOME"; then
  plugindir="$HOME/.gstreamer-$GST_MAJORMINOR/plugins"
else
  plugindir="\$(libdir)/gstreamer-$GST_MAJORMINOR"
fi
AC_SUBST(plugindir)


dnl set proper LDFLAGS etc for plugins
LDFLAGS='-module -avoid-version -export-symbols-regex [_]*\(gst_\|Gst\|GST_\).*'
AC_SUBST(LDFLAGS)

dnl Set component compilation flags
CFLAGS="$CFLAGS $MPF_CORE_CFLAGS $MPF_IPLIMAGE_CFLAGS $MPF_RDF_CFLAGS -Werror -g"
LIBS="$LIBADD $MPF_CORE_LIBS $MPF_IPLIMAGE_LIBS $MPF_RDF_LIBS"



dnl Add src/Makefile if you want to put your files in a subdirectory
AC_OUTPUT([
  Makefile
  mpf-opencv-opticalflow.spec
])
