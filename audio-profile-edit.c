/* audio-profile-edit.c: dialog to edit a specific profile */

/*
 * Copyright (C) 2003 Thomas Vander Stichele
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include "libgnome/gnome-i18n.h"
#include "gmp-util.h"
#include "audio-profile-edit.h"
#include "audio-profile.h"
#include "audio-profile-private.h"
#include <gtk/gtk.h>
#include <glade/glade-xml.h>

struct _GMAudioProfileEditPrivate
{
  GConfClient *conf;
  GladeXML *xml;
  AudioProfile *profile;
  GtkWidget *content;
};

static GObject*
gm_audio_profile_edit_constructor (GType type,
                                   guint n_construct_properties,
                                   GObjectConstructParam *construct_properties);

static void	gm_audio_profile_edit_init		(GMAudioProfileEdit *edit);
static void	gm_audio_profile_edit_class_init	(GMAudioProfileEditClass *klass);
static void	gm_audio_profile_edit_finalize		(GObject *object);
static void	gm_audio_profile_edit_destroy		(GtkObject *object);

static GtkWidget*
		gm_audio_profile_edit_get_widget	(GMAudioProfileEdit *dialog,
							 const char *widget_name);
static void	gm_audio_profile_edit_update_name	(GMAudioProfileEdit *dialog,
							 AudioProfile *profile);
static void	gm_audio_profile_edit_update_description
							(GMAudioProfileEdit *dialog,
							 AudioProfile *profile);
static void	gm_audio_profile_edit_update_pipeline	(GMAudioProfileEdit *dialog,
							 AudioProfile *profile);
static void	gm_audio_profile_edit_update_extension	(GMAudioProfileEdit *dialog,
							 AudioProfile *profile);
static void	gm_audio_profile_edit_update_active	(GMAudioProfileEdit *dialog,
							 AudioProfile *profile);
static void	on_profile_changed			(AudioProfile *profile,
							 const AudioSettingMask *mask,
							 GMAudioProfileEdit *dialog);

static gpointer parent_class;

GType
gm_audio_profile_edit_get_type (void)
{
  static GType object_type = 0;

  g_type_init ();

  if (!object_type)
  {
    static const GTypeInfo object_info =
    {
      sizeof (GMAudioProfileEditClass),
      (GBaseInitFunc) NULL,
      (GBaseFinalizeFunc) NULL,
      (GClassInitFunc) gm_audio_profile_edit_class_init,
      NULL,           /* class_finalize */
      NULL,           /* class_data */
      sizeof (GMAudioProfileEdit),
      0,              /* n_preallocs */
      (GInstanceInitFunc) gm_audio_profile_edit_init,
    };

    object_type = g_type_register_static (GTK_TYPE_DIALOG,
                                          "GMAudioProfileEdit",
                                          &object_info, 0);
  }

return object_type;
}

/* widget callbacks */

//FIXME
static void
fix_button_align (GtkWidget *button)
{
  GtkWidget *child;

  child = gtk_bin_get_child (GTK_BIN (button));

  if (GTK_IS_ALIGNMENT (child))
    g_object_set (G_OBJECT (child), "xalign", 0.0, NULL);
  else if (GTK_IS_LABEL (child))
    g_object_set (G_OBJECT (child), "xalign", 0.0, NULL);
}

/* ui callbacks */

/* FIXME: do we need this ? */
static void
gap_remove (GtkWidget *widget, gpointer *data)
{
  gtk_container_remove (GTK_CONTAINER (data), widget);
}

/* initialize a dialog widget from the glade xml file */
static void
gm_audio_profile_edit_init (GMAudioProfileEdit *dialog)
{
  GladeXML *xml;
  GtkWidget *vbox;

  dialog->priv = g_new0 (GMAudioProfileEditPrivate, 1);
}

static void
gm_audio_profile_edit_class_init (GMAudioProfileEditClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  /* FIXME: this code tries to override the constructor so it doesn't
     actually create the gtkdialog yet ! */
//  object_class->constructor = gm_audio_profile_edit_constructor;
  object_class->finalize = gm_audio_profile_edit_finalize;
}

static GObject*
gm_audio_profile_edit_constructor (GType                  type,
                              guint                  n_construct_properties,
                              GObjectConstructParam *construct_properties)
{
  return NULL;
}

static void
gm_audio_profile_edit_finalize (GObject *object)
{
  GMAudioProfileEdit *dialog;

  dialog = GM_AUDIO_PROFILE_EDIT (object);

  g_free (dialog->priv);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

/* ui callbacks */
static void
on_gm_audio_profile_edit_response (GMAudioProfileEdit *dialog,
                              int        id,
                              void      *data)
{
  if (id == GTK_RESPONSE_HELP)
    {
      GError *err;
      err = NULL;
/*
      gnome_help_display ("gnome-audio-profiles",
                          "gnome-audio-profiles-profile-edit",
                          &err);
*/
      if (err)
        {
          gmp_util_show_error_dialog  (GTK_WINDOW (dialog), NULL,
                                       _("There was an error displaying help: %s"), err->message);
          g_error_free (err);
        }
    }
  else
    {
      /* FIXME: hide or destroy ? */
      gtk_widget_hide (GTK_WIDGET (dialog));
    }
}

static void
on_gm_audio_profile_edit_destroy (GtkWidget *dialog, AudioProfile *profile)
{
  g_signal_handlers_disconnect_by_func (G_OBJECT (profile),
                                        G_CALLBACK (on_profile_changed),
                                        dialog);
}
/* profile callbacks */

static void
on_profile_changed (AudioProfile           *profile,
                    const AudioSettingMask *mask,
                    GMAudioProfileEdit         *dialog)
{
  if (mask->name)
    gm_audio_profile_edit_update_name (dialog, profile);
  if (mask->description)
    gm_audio_profile_edit_update_description (dialog, profile);
  if (mask->pipeline)
    gm_audio_profile_edit_update_pipeline (dialog, profile);
  if (mask->extension)
    gm_audio_profile_edit_update_extension (dialog, profile);
  if (mask->active)
    gm_audio_profile_edit_update_active (dialog, profile);
}

/* ui callbacks */
static void
on_profile_name_changed (GtkWidget       *entry,
                               AudioProfile *profile)
{
  char *text;

  text = gtk_editable_get_chars (GTK_EDITABLE (entry), 0, -1);

  audio_profile_set_name (profile, text);

  g_free (text);
}

static void
on_profile_description_changed (GtkWidget       *entry,
                                AudioProfile *profile)
{
  char *text;

  text = gtk_editable_get_chars (GTK_EDITABLE (entry), 0, -1);

  audio_profile_set_description (profile, text);

  g_free (text);
}

static void
on_profile_pipeline_changed (GtkWidget       *entry,
                             AudioProfile *profile)
{
  char *text;

  text = gtk_editable_get_chars (GTK_EDITABLE (entry), 0, -1);

  audio_profile_set_pipeline (profile, text);

  g_free (text);
}

static void
on_profile_extension_changed (GtkWidget       *entry,
                                AudioProfile *profile)
{
  char *text;

  text = gtk_editable_get_chars (GTK_EDITABLE (entry), 0, -1);

  audio_profile_set_extension (profile, text);

  g_free (text);
}

static void
on_profile_active_toggled (GtkWidget *button, AudioProfile *profile)
{
  audio_profile_set_active (profile, gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (button)));
}

/* create and return a new Profile Edit Dialog
 * given the GConf connection and the id of the profile
 */
GtkWidget*
gm_audio_profile_edit_new (GConfClient *conf, const char *id)
{
  GMAudioProfileEdit *dialog;
  GladeXML *xml;
  GtkWidget *w;

  /* get the dialog */
  xml = gmp_util_load_glade_file (GM_AUDIO_GLADE_FILE,
                                  "profile-edit-dialog", NULL);
  dialog = (GMAudioProfileEdit *) glade_xml_get_widget (xml, "profile-edit-dialog");

  /* make sure we have priv */
  if (dialog->priv == NULL)
  {
    /* we didn't go through _init; this chould happen for example when
     * we specified profile-edit-dialog as a GtkDialog instead of a
     * GMAudioProfileEdit for easy glade editing */
    /* FIXME: to be honest, this smells like I'm casting a created GtkDialog
     * widget to a GapEditProfile and then doing stuff to it.  That doesn't
     * smell to good to me */
    dialog->priv = g_new0 (GMAudioProfileEditPrivate, 1);
  }
  dialog->priv->xml = xml;

  /* save the GConf stuff and get the profile belonging to this id */
  g_object_ref (G_OBJECT (conf));
  dialog->priv->conf = conf;

  dialog->priv->profile = audio_profile_lookup (id);
  g_assert (dialog->priv->profile);
  /* connect callbacks */
  g_signal_connect (G_OBJECT (dialog),
                    "response",
                    G_CALLBACK (on_gm_audio_profile_edit_response),
                    dialog);

  g_signal_connect (G_OBJECT (dialog),
                    "destroy",
                    G_CALLBACK (on_gm_audio_profile_edit_destroy),
                    dialog);

  /* autoconnect doesn't handle data pointers, sadly, so do by hand */
  w = glade_xml_get_widget (xml, "profile-name-entry");
  gm_audio_profile_edit_update_name (dialog, dialog->priv->profile);
  g_signal_connect (G_OBJECT (w), "changed",
                    G_CALLBACK (on_profile_name_changed), dialog->priv->profile);
  w = glade_xml_get_widget (xml, "profile-description-entry");
  gm_audio_profile_edit_update_description (dialog, dialog->priv->profile);
  g_signal_connect (G_OBJECT (w), "changed",
                    G_CALLBACK (on_profile_description_changed), dialog->priv->profile);
   w = glade_xml_get_widget (xml, "profile-pipeline-entry");
  gm_audio_profile_edit_update_pipeline (dialog, dialog->priv->profile);
  g_signal_connect (G_OBJECT (w), "changed",
                    G_CALLBACK (on_profile_pipeline_changed), dialog->priv->profile);
  w = glade_xml_get_widget (xml, "profile-extension-entry");
  gm_audio_profile_edit_update_extension (dialog, dialog->priv->profile);
  g_signal_connect (G_OBJECT (w), "changed",
                    G_CALLBACK (on_profile_extension_changed), dialog->priv->profile);
  w = glade_xml_get_widget (xml, "profile-active-button");
  gm_audio_profile_edit_update_active (dialog, dialog->priv->profile);
  g_signal_connect (G_OBJECT (w), "toggled",
                    G_CALLBACK (on_profile_active_toggled), dialog->priv->profile);


  gtk_window_set_destroy_with_parent (GTK_WINDOW (dialog), TRUE);

  /* connect to profile changes */

  g_signal_connect (G_OBJECT (dialog->priv->profile),
                    "changed",
                    G_CALLBACK (on_profile_changed),
                    dialog);

  gtk_window_present (GTK_WINDOW (dialog));

  return GTK_WIDGET (dialog);
}

/* UI consistency update functions */
static void
entry_set_text_if_changed (GtkEntry   *entry,
                           const char *text)
{
  char *s;

  GMP_DEBUG("entry_set_text_if_changed on entry %p with text %s\n", entry, text);
  s = gtk_editable_get_chars (GTK_EDITABLE (entry), 0, -1);
  GMP_DEBUG("got editable text %s\n", s);
  if (text && strcmp (s, text) != 0)
    gtk_entry_set_text (GTK_ENTRY (entry), text);
  GMP_DEBUG("entry_set_text_if_changed: got %s\n", s);

  g_free (s);
}

static void
gm_audio_profile_edit_update_name (GMAudioProfileEdit *dialog,
                              AudioProfile *profile)
{
  char *s;
  GtkWidget *w;

  s = g_strdup_printf (_("Editing profile \"%s\""),
                       audio_profile_get_name (profile));
  GMP_DEBUG("g_p_e_u_n: title %s\n", s);

  gtk_window_set_title (GTK_WINDOW (dialog), s);

  g_free (s);

  w = gm_audio_profile_edit_get_widget (dialog, "profile-name-entry");
  g_assert (GTK_IS_WIDGET (w));

  entry_set_text_if_changed (GTK_ENTRY (w),
                             audio_profile_get_name (profile));
}

static void
gm_audio_profile_edit_update_description (GMAudioProfileEdit *dialog,
                              AudioProfile *profile)
{
  GtkWidget *w;

  w = gm_audio_profile_edit_get_widget (dialog, "profile-description-entry");
  g_assert (GTK_IS_WIDGET (w));

  entry_set_text_if_changed (GTK_ENTRY (w),
                             audio_profile_get_description (profile));
}

static void
gm_audio_profile_edit_update_pipeline (GMAudioProfileEdit *dialog,
                              AudioProfile *profile)
{
  GtkWidget *w;

  w = gm_audio_profile_edit_get_widget (dialog, "profile-pipeline-entry");
  g_assert (GTK_IS_WIDGET (w));

  entry_set_text_if_changed (GTK_ENTRY (w),
                             audio_profile_get_pipeline (profile));
}

static void
gm_audio_profile_edit_update_extension (GMAudioProfileEdit *dialog,
                              AudioProfile *profile)
{
  GtkWidget *w;

  w = gm_audio_profile_edit_get_widget (dialog, "profile-extension-entry");
  g_assert (GTK_IS_WIDGET (w));

  entry_set_text_if_changed (GTK_ENTRY (w),
                             audio_profile_get_extension (profile));
}

static void
gm_audio_profile_edit_update_active (GMAudioProfileEdit *dialog,
                                AudioProfile *profile)
{
  GtkWidget *w;

  w = gm_audio_profile_edit_get_widget (dialog, "profile-active-button");
  g_assert (GTK_IS_WIDGET (w));

  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w),
                                audio_profile_get_active (profile));
}

static GtkWidget*
gm_audio_profile_edit_get_widget (GMAudioProfileEdit *dialog,
                             const char *widget_name)
{
  GladeXML *xml;
  GtkWidget *w;

  xml = dialog->priv->xml;

  g_return_val_if_fail (xml, NULL);

  w = glade_xml_get_widget (xml, widget_name);

  if (w == NULL)
    g_error ("No such widget %s", widget_name);

  return w;
}

