glademoduledir = $(libdir)/libglade/2.0
glademodule_LTLIBRARIES = libgnome-media-profiles.la

defines = \
	-DG_LOG_DOMAIN=\"gnome-media-profiles\" \
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\"

libgnome_media_profiles_la_SOURCES = gnome-media-profiles-glade.c
libgnome_media_profiles_la_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
libgnome_media_profiles_la_LIBADD = $(GMP_LIBS) $(top_builddir)/profiles/libgnome-media-profiles.la
libgnome_media_profiles_la_LDFLAGS = -export-dynamic -module -avoid-version -no-undefined


-include $(top_srcdir)/git.mk
