#include <profiles/audio-profile-edit.h>
#include <glade/glade-init.h>
#include <glade/glade-build.h>


static GtkWidget *
gm_audio_profile_edit_new_2 (GladeXML *xml, GType widget_type,
                             GladeWidgetInfo *info)
{
    GtkWidget *dialog;

    dialog = glade_standard_build_widget (xml, widget_type, info);

    return dialog;
}

/* this macro puts a version check function into the module */
GLADE_MODULE_CHECK_INIT

void
glade_module_register_widgets (void)
{
  /* FIXME: Why is this here? gm_audio_profile_edit_get_type (); */
  glade_register_widget (GM_AUDIO_TYPE_PROFILE_EDIT,
                         gm_audio_profile_edit_new_2,
                         NULL, NULL);
  glade_provide ("gnome-media-profiles");
}
