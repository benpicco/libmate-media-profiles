# this is provided for editing gnome-media UI files, not for any other
# purposes

libgnome_media_profiles_la_SOURCES = gnome-media-profiles-catalog.c
gladecatalog_DATA_dist = gnome-media-profiles.xml

if HAVE_GLADEUI
glademoduledir = @GLADEUI_MODULE_DIR@
glademodule_LTLIBRARIES = libgnome-media-profiles.la

defines = \
	-DG_LOG_DOMAIN=\"gnome-media-profiles\" \
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\" \
	$(DISABLE_DEPRECATED)

libgnome_media_profiles_la_CFLAGS = $(GMP_CFLAGS) $(GLADEUI_CFLAGS) $(defines) -I$(top_srcdir)
libgnome_media_profiles_la_LIBADD = $(GMP_LIBS) $(GLADEUI_LIBS) $(top_builddir)/libgnome-media-profiles/libgnome-media-profiles-3.0.la
libgnome_media_profiles_la_LDFLAGS = -export-dynamic -module -avoid-version -no-undefined

gladecatalog_DATA = $(gladecatalog_DATA_dist)
gladecatalogdir = @GLADEUI_CATALOG_DIR@

check-catalog:
	xmllint --dtdvalid glade-catalog.dtd --noout $(gladecatalog_DATA)
endif

EXTRA_DIST = \
	$(gladecatalog_DATA_dist)

-include $(top_srcdir)/git.mk
