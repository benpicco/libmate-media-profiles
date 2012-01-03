
uidir = $(pkgdatadir)
ui_DATA = mate-audio-profile-new.ui gnome-audio-profile-edit.ui

@INTLTOOL_SCHEMAS_RULE@

schemasdir = $(MATECONF_SCHEMA_FILE_DIR)
schemas_in_files = gnome-media-profiles.schemas.in
schemas_DATA = $(schemas_in_files:.schemas.in=.schemas)

install-schemas:
	GCONF_CONFIG_SOURCE=$(GCONF_SCHEMA_CONFIG_SOURCE) $(GCONFTOOL) --makefile-install-rule $(top_builddir)/profiles/$(schema_DATA) || true

if GCONF_SCHEMAS_INSTALL
install-data-local: install-schemas
else
install-data-local:
endif

CLEANFILES = $(schemas_DATA)

EXTRA_DIST = $(ui_DATA) $(schema_DATA)

MAINTAINERCLEANFILES =			\
	Makefile.in

-include $(top_srcdir)/git.mk
