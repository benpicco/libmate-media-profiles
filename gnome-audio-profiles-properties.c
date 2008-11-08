/* gnome-audio-profiles-properties.c:
   properties capplet that shows the GapProfilesEdit dialog */

/*
 * Copyright (C) 2003 Thomas Vander Stichele
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libintl.h>
#include <gtk/gtkmain.h>
#include <libgnome/gnome-program.h>
#include <libgnomeui/gnome-ui-init.h>

#include "gnome-media-profiles.h"
#include "audio-profile-private.h"

static void
on_dialog_destroy (GtkWidget *dialog, gpointer *user_data)
{
  /* dialog destroyed, time to bail */
  gtk_main_quit ();
}

int
main (int argc, char *argv[])
{
  GtkWidget *widget;
  GConfClient *conf;
  GnomeProgram *program;

  program = gnome_program_init ("gnome-audio-profiles-properties",
			        VERSION,
			        LIBGNOMEUI_MODULE,
			        argc, argv,
				GNOME_PARAM_APP_DATADIR, DATADIR,
			        GNOME_PARAM_NONE);

  /* FIXME: add a comment why we need this at all, until then
     we comment it out
  gm_audio_profile_edit_get_type (); */
  glade_register_widget (gm_audio_profile_edit_get_type (),
			 NULL,
			 gtk_dialog_build_children,
			 dialog_find_internal_child);
  conf = gconf_client_get_default ();
  textdomain (GETTEXT_PACKAGE);
  gnome_media_profiles_init (conf);

  gtk_window_set_default_icon_name ("gnome-mime-audio");

  widget = GTK_WIDGET (gm_audio_profiles_edit_new (conf, NULL));
  g_assert (GTK_IS_WIDGET (widget));

  g_signal_connect (G_OBJECT (widget), "destroy",
                    G_CALLBACK (on_dialog_destroy), NULL);

  gtk_widget_show_all (widget);
  gtk_main ();

  g_object_unref (conf);
  g_object_unref (program);

  return 0;
}
