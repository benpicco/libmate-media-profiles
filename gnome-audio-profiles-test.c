#include "config.h"
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <gconf/gconf-client.h>
#include <libgnomeui/libgnomeui.h>
#include <profiles/gnome-media-profiles.h>

static gboolean
edit_cb (GtkButton *button, GtkWindow *window)
{
  GtkWidget *edit_dialog = NULL;
  edit_dialog = gm_audio_profiles_edit_new (gconf_client_get_default (), window);
  g_assert (edit_dialog != NULL);
  gtk_widget_show_all (GTK_WIDGET (edit_dialog));

  return FALSE;
}

static gboolean
test_cb (GtkButton *button, GtkWidget *combo)
{
  gchar *partialpipe = NULL;
  gchar *extension = NULL;
  gchar *pipeline_desc;
  GError *error = NULL;
  int i;
  GstElement *pipeline;
  GMAudioProfile *profile;


  profile = gm_audio_profile_choose_get_active (combo);
  extension = g_strdup (gm_audio_profile_get_extension (profile));
  partialpipe = g_strdup (gm_audio_profile_get_pipeline (profile));

  g_print ("You chose profile with name %s and pipeline %s\n",
           gm_audio_profile_get_name (profile),
           gm_audio_profile_get_pipeline (profile));

  pipeline_desc = g_strdup_printf ("sinesrc ! audioconvert ! %s ! filesink location=test.%s",
                                   partialpipe, extension);
  g_print ("Going to run pipeline %s\n", pipeline_desc);

  pipeline = gst_parse_launch (pipeline_desc, &error);
  if (error)
  {
    g_print ("Error parsing pipeline: %s\n", error->message);
    g_error_free (error);
    return FALSE;
  }

  gst_element_set_state (pipeline, GST_STATE_PLAYING);
  g_print ("Writing test sound to test.%s\n", extension);
  for (i = 0; i < 100; ++i)
    gst_bin_iterate (GST_BIN (pipeline));
  gst_element_set_state (pipeline, GST_STATE_NULL);
  g_free (pipeline);
  g_free (extension);
  g_free (partialpipe);

  return FALSE;
}

int
main (int argc, char **argv)
{
  GtkWidget *window, *hbox, *combo, *edit, *test;
  GConfClient *gconf;

  gst_init (&argc, &argv);
  gnome_program_init ("gnome-audio-profiles-test", VERSION,
                      LIBGNOMEUI_MODULE, argc, argv,
                      NULL);

  gconf = gconf_client_get_default ();
  gnome_media_profiles_init (gconf);

  window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  combo = gm_audio_profile_choose_new ();

  edit = gtk_button_new_with_mnemonic ("_Edit Profiles");
  test = gtk_button_new_with_mnemonic ("_Test");
  g_signal_connect (edit, "clicked", (GCallback) edit_cb, window);
  g_signal_connect (test, "clicked", (GCallback) test_cb, combo);
  g_signal_connect (edit, "destroy", (GCallback) gtk_main_quit, NULL);

  hbox = gtk_hbox_new (FALSE, 7);
  gtk_box_pack_start (GTK_BOX (hbox), combo, TRUE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), test, FALSE, TRUE, 0);
  gtk_box_pack_start (GTK_BOX (hbox), edit, FALSE, TRUE, 0);
  gtk_container_add (GTK_CONTAINER (window), hbox);
  gtk_widget_show_all (window);
  gtk_main ();

  return 0;
}
