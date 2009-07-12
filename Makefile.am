bin_PROGRAMS = gnome-audio-profiles-properties
noinst_PROGRAMS =

defines = \
	-DG_LOG_DOMAIN=\"gnome-media-profiles\" \
	-DDATADIR="\"$(pkgdatadir)\"" \
	-DGMP_UIDIR=\"$(uidir)\" \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"

if HAVE_GSTREAMER
noinst_PROGRAMS += gnome-audio-profiles-test
gnome_audio_profiles_test_SOURCES = gnome-audio-profiles-test.c
gnome_audio_profiles_test_CFLAGS = $(GMP_CFLAGS) $(GSR_CFLAGS) $(defines) -I$(top_srcdir)
gnome_audio_profiles_test_LDADD = libgnome-media-profiles.la $(GMP_LIBS) $(GSR_LIBS)
endif

lib_LTLIBRARIES = libgnome-media-profiles.la

@INTLTOOL_DESKTOP_RULE@
@INTLTOOL_SCHEMAS_RULE@

uidir = $(pkgdatadir)
ui_DATA = gnome-audio-profile-new.ui gnome-audio-profile-edit.ui

gnome_audio_profiles_properties_SOURCES = gnome-audio-profiles-properties.c
gnome_audio_profiles_properties_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
gnome_audio_profiles_properties_LDADD = libgnome-media-profiles.la $(GMP_LIBS)

libgnome_media_profiles_la_SOURCES = \
	audio-profile.c \
	audio-profiles-edit.c \
	audio-profile-edit.c \
	audio-profile-choose.c \
	gnome-media-profiles.c \
	gmp-util.c

noinst_HEADERS = \
	audio-profile-private.h \
	gmp-conf.h \
	gmp-util.h


gmpincludedir = $(includedir)/gnome-media/profiles
gmpinclude_HEADERS = \
	audio-profile.h \
	audio-profile-choose.h \
	audio-profile-edit.h \
	audio-profiles-edit.h \
	gnome-media-profiles.h

libgnome_media_profiles_la_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
libgnome_media_profiles_la_LIBADD = $(GMP_LIBS)
libgnome_media_profiles_la_LDFLAGS = -no-undefined

schemadir = $(GCONF_SCHEMA_FILE_DIR)
schema_DATA = gnome-audio-profiles.schemas

install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/profiles/$(schema_DATA) || true

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
else
install-data-local:
endif

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = gnome-media-profiles.pc
EXTRA_DIST = $(ui_DATA) $(schema_DATA) $(pkgconfig_DATA)

CLEANFILES = $(schema_DATA)

# "." is dirty hack to first build our .la in this dir before moving to glade
SUBDIRS =  . help

if HAVE_GLADEUI
SUBDIRS += glade
endif

-include $(top_srcdir)/git.mk
