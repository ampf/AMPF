AC_INIT

PACKAGE="grdf"
VERSION_MAJOR=0
VERSION_MINOR=0
VERSION_MICRO=3
VERSION_NANO=0

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

dnl Bring in check.h
PKG_CHECK_MODULES([CHECK], [check >= 0.9.4])

dnl Check for pkgconfig first
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, yes, no)

dnl Give error and exit if we don't have pkgconfig
if test "x$HAVE_PKGCONFIG" = "xno"; then
  AC_MSG_ERROR(you need to have pkgconfig installed !)
fi


dnl Get the various flags needed to link against GObject
PKG_CHECK_MODULES(GOBJECT, gobject-2.0,
  , AC_MSG_ERROR(you need gobject development packages installed !))

dnl make GST_CFLAGS and GST_LIBS available
AC_SUBST(GOBJECT_CFLAGS)
AC_SUBST(GOBJECT_LIBS)



dnl Add src/Makefile if you want to put your files in a subdirectory
AC_OUTPUT([
  Makefile
  src/Makefile
  test/Makefile
  grdf.spec
  grdf.pc
  grdf-uninstalled.pc
])
