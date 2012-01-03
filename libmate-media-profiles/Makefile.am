bin_PROGRAMS = mate-audio-profiles-properties
noinst_PROGRAMS =

uidir = $(pkgdatadir)

defines = \
	-DG_LOG_DOMAIN=\"mate-media-profiles\" \
	-DDATADIR="\"$(pkgdatadir)\"" \
	-DGMP_UIDIR=\"$(uidir)\" \
	-DMATELOCALEDIR=\""$(datadir)/locale"\" \
	$(DISABLE_DEPRECATED)

if HAVE_GSTREAMER
noinst_PROGRAMS += mate-audio-profiles-test
mate_audio_profiles_test_SOURCES = mate-audio-profiles-test.c
mate_audio_profiles_test_CFLAGS = $(GMP_CFLAGS) $(GSR_CFLAGS) $(defines) -I$(top_srcdir)
mate_audio_profiles_test_LDADD = libmate-media-profiles-3.0.la $(GMP_LIBS) $(GSR_LIBS)
endif

lib_LTLIBRARIES = libmate-media-profiles-3.0.la

mate_audio_profiles_properties_SOURCES = mate-audio-profiles-properties.c
mate_audio_profiles_properties_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
mate_audio_profiles_properties_LDADD = libmate-media-profiles-3.0.la $(GMP_LIBS)

libmate_media_profiles_3_0_la_SOURCES = \
	audio-profile.c \
	audio-profiles-edit.c \
	audio-profile-edit.c \
	audio-profile-choose.c \
	mate-media-profiles.c \
	gmp-util.c

noinst_HEADERS = \
	audio-profile-private.h \
	gmp-conf.h \
	gmp-util.h


gmpincludedir = $(includedir)/libmate-media-profiles-3.0/libmate-media-profiles
gmpinclude_HEADERS = \
	audio-profile.h \
	audio-profile-choose.h \
	audio-profile-edit.h \
	audio-profiles-edit.h \
	mate-media-profiles.h

libmate_media_profiles_3_0_la_CFLAGS = $(GMP_CFLAGS) $(defines) -I$(top_srcdir)
libmate_media_profiles_3_0_la_LIBADD = $(GMP_LIBS)
libmate_media_profiles_3_0_la_LDFLAGS = -no-undefined

-include $(top_srcdir)/git.mk
