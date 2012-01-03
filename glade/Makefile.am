# this is provided for editing mate-media UI files, not for any other
# purposes

libmate_media_profiles_la_SOURCES = mate-media-profiles-catalog.c
gladecatalog_DATA_dist = mate-media-profiles.xml

if HAVE_GLADEUI
glademoduledir = @GLADEUI_MODULE_DIR@
glademodule_LTLIBRARIES = libmate-media-profiles.la

defines = \
	-DG_LOG_DOMAIN=\"mate-media-profiles\" \
	-DMATELOCALEDIR=\""$(prefix)/${DATADIRNAME}/locale"\" \
	$(DISABLE_DEPRECATED)

libmate_media_profiles_la_CFLAGS = $(GMP_CFLAGS) $(GLADEUI_CFLAGS) $(defines) -I$(top_srcdir)
libmate_media_profiles_la_LIBADD = $(GMP_LIBS) $(GLADEUI_LIBS) $(top_builddir)/libmate-media-profiles/libmate-media-profiles.la
libmate_media_profiles_la_LDFLAGS = -export-dynamic -module -avoid-version -no-undefined

gladecatalog_DATA = $(gladecatalog_DATA_dist)
gladecatalogdir = @GLADEUI_CATALOG_DIR@

check-catalog:
	xmllint --dtdvalid glade-catalog.dtd --noout $(gladecatalog_DATA)
endif

EXTRA_DIST = \
	$(gladecatalog_DATA_dist)

-include $(top_srcdir)/git.mk
