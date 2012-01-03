bin_PROGRAMS = mate-audio-profiles-properties
noinst_PROGRAMS =

uidir = $(pkgdatadir)

defines = \
	-DG_LOG_DOMAIN=\"gnome-media-profiles\" \
	-DDATADIR="\"$(pkgdatadir)\"" \
	-DGMP_UIDIR=\"$(uidir)\" \
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	$(DISABLE_DEPRECATED)

if HAVE_GSTREAMER
noinst_PROGRAMS += gnome-audio-profiles-test
gnome_audio_profiles_test_SOURCES = gnome-audio-profiles-test.c
gnome_audio_profiles_test_CFLAGS = $(GMP_CFLAGS) $(GSR_CFLAGS) $(defines) -I$(top_srcdir)
gnome_audio_profiles_test_LDADD = libgnome-media-profiles-3.0.la $(GMP_LIBS) $(GSR_LIBS)
endif

lib_LTLIBRARIES = libgnome-media-profiles-3.0.la

gnome_audio_profiles_properties_SOURCES = gnome-audio-profiles-properties.c
gnome_audio_profiles_properties_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
gnome_audio_profiles_properties_LDADD = libgnome-media-profiles-3.0.la $(GMP_LIBS)

libgnome_media_profiles_3_0_la_SOURCES = \
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


gmpincludedir = $(includedir)/libgnome-media-profiles-3.0/libgnome-media-profiles
gmpinclude_HEADERS = \
	audio-profile.h \
	audio-profile-choose.h \
	audio-profile-edit.h \
	audio-profiles-edit.h \
	gnome-media-profiles.h

libgnome_media_profiles_3_0_la_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
libgnome_media_profiles_3_0_la_LIBADD = $(GMP_LIBS)
libgnome_media_profiles_3_0_la_LDFLAGS = -no-undefined

-include $(top_srcdir)/git.mk
