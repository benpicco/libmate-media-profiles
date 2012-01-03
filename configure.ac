dnl Process this file with autoconf to produce a configure script.
AC_PREREQ(2.53)

AC_INIT([libmate-media-profiles], [3.0.0],
        [http://bugzilla.mate.org/enter_bug.cgi?product=libmate-media-profiles])

AM_INIT_AUTOMAKE([1.9 tar-ustar foreign dist-bzip2 no-dist-gzip])

PKG_PROG_PKG_CONFIG([0.19])
AC_PROG_SED
IT_PROG_INTLTOOL([0.35.0])

AC_PATH_PROG(MATECONFTOOL, mateconftool-2)
AM_GCONF_SOURCE_2

GETTEXT_PACKAGE=libmate-media-profiles
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Define to the Gettext package name])
AM_GLIB_GNU_GETTEXT

MATE_DOC_INIT

AM_PROG_LIBTOOL
AM_PROG_CC_C_O

# Workaround to make aclocal get the right flags
AC_SUBST(ACLOCAL_AMFLAGS, "\${ACLOCAL_FLAGS}")

GTK_REQUIRED_VERSION=2.91.0
PKG_CHECK_MODULES(GMP, [
    gtk+-3.0 >= $GTK_REQUIRED_VERSION
    gstreamer-0.10
    gconf-2.0
    gio-2.0],
    have_profiles=yes)

AC_SUBST(GMP_CFLAGS)
AC_SUBST(GMP_LIBS)
AM_CONDITIONAL(HAVE_GMP, [test x$have_profiles = xyes])

dnl Profiles glade ui: needed to edit .ui files with glade3
have_gladeui=no

# gladeui-3.0 is hypothetical
PKG_CHECK_MODULES(GLADEUI, [gladeui-2.0], have_gladeui=yes, have_gladeui=no)
AC_SUBST(GLADEUI_CFLAGS)
AC_SUBST(GLADEUI_LIBS)
AM_CONDITIONAL(HAVE_GLADEUI, [test x$have_gladeui = xyes])

GLADEUI_CATALOG_DIR="`$PKG_CONFIG --variable=catalogdir gladeui-2.0`"
AC_SUBST(GLADEUI_CATALOG_DIR)

GLADEUI_MODULE_DIR="`$PKG_CONFIG --variable=moduledir gladeui-2.0`"
AC_SUBST(GLADEUI_MODULE_DIR)

dnl=======================================================================
dnl Check for GStreamer
dnl=======================================================================

GST_REQ=0.10.23
GST_PLUGINS_BASE_REQ=0.10.23
GST_MAJORMINOR=0.10

dnl make available to Makefile.am
AC_SUBST(GST_MAJORMINOR)
AC_DEFINE_UNQUOTED(GST_MAJORMINOR_QUOTED, "$GST_MAJORMINOR",
  [major/minor of GStreamer as a string constant])

have_gst=no
PKG_CHECK_MODULES(GST, [
  gstreamer-$GST_MAJORMINOR >= $GST_REQ
  gstreamer-plugins-base-$GST_MAJORMINOR >= $GST_REQ
  gtk+-3.0 >= 2.6.0
  gio-2.0
  gconf-2.0],
  have_gst=yes,
  [
    AC_MSG_RESULT(no)
    AC_MSG_WARN($GST_PKG_ERRORS)
  ])

AC_SUBST(GST_CFLAGS)
AC_SUBST(GST_LIBS)
AM_CONDITIONAL(HAVE_GSTREAMER, [test x$have_gst = xyes])


AC_CONFIG_FILES([
libmate-media-profiles.pc
Makefile
libmate-media-profiles/Makefile
data/Makefile
data/mate-media-profiles.schemas.in
glade/Makefile
help/Makefile
po/Makefile.in
])

AC_OUTPUT
