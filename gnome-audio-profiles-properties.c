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

#include <config.h>
#include <gtk/gtkmain.h>
#include "gnome-media-profiles.h"
#include <glade/glade.h>
#include <glade/glade-build.h>
#include <string.h>

static void
on_dialog_destroy (GtkWidget *dialog, gpointer *user_data)
{
  /* dialog destroyed, time to bail */
  gtk_main_quit ();
}

static void
gtk_dialog_build_children(GladeXML *self, GtkWidget *w,
			  GladeWidgetInfo *info)

{
    GtkDialog *dialog = GTK_DIALOG (w);
    GList *children, *list;

    glade_standard_build_children (self, w, info);

    if (dialog->action_area == NULL)
	return;

    /* repack children of action_area */
    children = gtk_container_get_children(GTK_CONTAINER(dialog->action_area));
    for (list = children; list; list = list->next) {
	GtkWidget *child = GTK_WIDGET(list->data);

	g_object_ref(child);
	gtk_container_remove (GTK_CONTAINER (dialog->action_area), child);
    }
    for (list = children; list; list = list->next) {
	GtkWidget *child = GTK_WIDGET(list->data);
	gint response_id;

	response_id = GPOINTER_TO_INT(g_object_steal_data(G_OBJECT(child),
							  "response_id"));
	gtk_dialog_add_action_widget(dialog, child, response_id);
	g_object_unref(child);

    }
    g_list_free (children);
}

static GtkWidget *
dialog_find_internal_child(GladeXML *xml, GtkWidget *parent,
			   const gchar *childname)
{
    if (!strcmp(childname, "vbox"))
	return GTK_DIALOG(parent)->vbox;
    if (!strcmp(childname, "action_area"))
	return GTK_DIALOG(parent)->action_area;

    return NULL;
}

int
main (int argc, char *argv[])
{
  GtkWidget *widget;
  static GConfClient *conf;

  gtk_init (&argc, &argv);
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

  widget = GTK_WIDGET (gm_audio_profiles_edit_new (conf, NULL));
  g_assert (GTK_IS_WIDGET (widget));

  g_signal_connect (G_OBJECT (widget), "destroy",
                    G_CALLBACK (on_dialog_destroy), NULL);

  gtk_widget_show_all (widget);
  gtk_main ();

  return 0;
}
