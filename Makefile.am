bin_PROGRAMS = gnome-audio-profiles-properties
lib_LTLIBRARIES = libgnome-media-profiles.la

@INTLTOOL_DESKTOP_RULE@
@INTLTOOL_SCHEMAS_RULE@

gladedir = $(pkgdatadir)/glade
glade_DATA = gnome-audio-profiles.glade2

defines = \
	-DG_LOG_DOMAIN=\"gnome-media-profiles\" \
	-DGMP_GLADE_DIR=\"$(gladedir)\" \
	-DGM_AUDIO_GLADE_FILE=\"gnome-audio-profiles.glade2\" \
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\"


gnome_audio_profiles_properties_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
gnome_audio_profiles_properties_LDADD = $(GMP_LIBS) libgnome-media-profiles.la

libgnome_media_profiles_la_SOURCES = \
	audio-profile.c \
	audio-profiles-edit.c \
	audio-profile-edit.c \
	gnome-media-profiles.c \
	gmp-util.c

noinst_HEADERS = \
	audio-profile.h \
	gmp-conf.h \
	gmp-util.h


gmpincludedir = $(includedir)/gnome-media
gmpinclude_HEADERS = \
	audio-profile-edit.h \
	audio-profiles-edit.h \
	gnome-media-profiles.h

libgnome_media_profiles_la_CFLAGS = $(GMP_CFLAGS) $(defines)
libgnome_media_profiles_la_LIBADD = $(GMP_LIBS)

schemadir = $(sysconfdir)/gconf/schemas
schema_DATA = gnome-audio-profiles.schemas

install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/profiles/$(schema_DATA) || true

install-data-local: install-schemas

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnome-media-profiles.pc

EXTRA_DIST = $(glade_DATA) $(schema_DATA) $(pkgconfig_DATA)

CLEANFILES = $(schema_DATA)

# "." is dirty hack to first build our .la in this dir before moving to glade
SUBDIRS =  . glade
