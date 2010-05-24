AC_INIT

GST_MAJORMINOR=0.10
GST_REQUIRED=0.10.0

AM_MAINTAINER_MODE
AM_INIT_AUTOMAKE($VERSION, $PACKAGE)

AM_CONFIG_HEADER(config.h)

AC_PROG_LIBTOOL

AC_CONFIG_MACRO_DIR(m4)
GTK_DOC_CHECK(1.9)

dnl Check for pkgconfig first
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, yes, no)

dnl Give error and exit if we don't have pkgconfig
if test "x$HAVE_PKGCONFIG" = "xno"; then
  AC_MSG_ERROR(You need pkgconfig installed!)
fi

dnl Now we're ready to ask for gstreamer libs and cflags
dnl And we can also ask for the right version of gstreamer

PKG_CHECK_MODULES(GST, \
  gstreamer-$GST_MAJORMINOR >= $GST_REQUIRED,
  HAVE_GST=yes,HAVE_GST=no)

dnl Give error and exit if we don't have gstreamer
if test "x$HAVE_GST" = "xno"; then
  AC_MSG_ERROR(You need GStreamer development packages installed!)
fi

GST_CFLAGS="$GST_CFLAGS $GST_ERROR"
AC_SUBST(GST_CFLAGS)
AC_SUBST(GST_LIBS)

AC_CONFIG_FILES([
  Makefile
  docs/Makefile
  docs/reference/Makefile
  docs/reference/common/Makefile
  docs/reference/mpf-core/Makefile
])
AC_OUTPUT
