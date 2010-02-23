# this is provided for editing gnome-media UI files, not for any other
# purposes

glademoduledir = @GLADEUI_MODULE_DIR@
glademodule_LTLIBRARIES = libgnome-media-profiles.la

defines = \
	-DG_LOG_DOMAIN=\"gnome-media-profiles\" \
	-DGNOMELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\"

libgnome_media_profiles_la_SOURCES = gnome-media-profiles-catalog.c
libgnome_media_profiles_la_CFLAGS = $(GMP_CFLAGS) $(GLADEUI_CFLAGS) $(defines) -I$(top_srcdir)
libgnome_media_profiles_la_LIBADD = $(GMP_LIBS) $(GLADEUI_LIBS) $(top_builddir)/profiles/libgnome-media-profiles.la
libgnome_media_profiles_la_LDFLAGS = -export-dynamic -module -avoid-version -no-undefined

gladecatalogdir = @GLADEUI_CATALOG_DIR@
gladecatalog_DATA = gnome-media-profiles.xml

check-catalog:
	xmllint --dtdvalid glade-catalog.dtd --noout $(gladecatalog_DATA)

EXTRA_DIST = \
	$(gladecatalog_DATA)

-include $(top_srcdir)/git.mk
