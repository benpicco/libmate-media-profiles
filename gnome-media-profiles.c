/* gnome-media-profiles.c: public library code */

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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "gnome-media-profiles.h"

#include <glade/glade.h>
#include <glade/glade-build.h>
#include <string.h>
#include <glib/gi18n.h>
#include <gconf/gconf-client.h>

#include "audio-profile-private.h"
#include "audio-profile-edit.h"
#include "gmp-conf.h"
#include "gmp-util.h"

void
gtk_dialog_build_children (GladeXML *self, GtkWidget *w,
			  GladeWidgetInfo *info)

{
  GtkDialog *dialog = GTK_DIALOG (w);
  GList *children, *list;

  glade_standard_build_children (self, w, info);

  if (dialog->action_area == NULL)
    return;

  /* repack children of action_area */
  children = gtk_container_get_children (GTK_CONTAINER (dialog->action_area));
  for (list = children; list; list = list->next) {
    GtkWidget *child = GTK_WIDGET (list->data);

    g_object_ref (child);
    gtk_container_remove (GTK_CONTAINER (dialog->action_area), child);
  }
  for (list = children; list; list = list->next) {
    GtkWidget *child = GTK_WIDGET (list->data);
    gint response_id;

    response_id = GPOINTER_TO_INT (g_object_steal_data (G_OBJECT(child),
						      "response_id"));
   gtk_dialog_add_action_widget (dialog, child, response_id);
   g_object_unref (child);
  }
  g_list_free (children);
}

GtkWidget *
dialog_find_internal_child (GladeXML *xml, GtkWidget *parent,
				   const gchar *childname)
{
  if (!strcmp (childname, "vbox"))
    return GTK_DIALOG(parent)->vbox;
  if (!strcmp (childname, "action_area"))
    return GTK_DIALOG (parent)->action_area;
  
  return NULL;
}

/* do all necessary initialization to use this simple helper library */
void
gnome_media_profiles_init (GConfClient *conf)
{
  GError *err = NULL;

  /* i18n */
  bindtextdomain (GETTEXT_PACKAGE, GNOMELOCALEDIR);
#ifdef HAVE_BIND_TEXTDOMAIN_CODESET
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
#endif

  if (conf == NULL) 
    conf = gconf_client_get_default ();
  else
    g_object_ref (G_OBJECT (conf));

    /* initialize GConf */
    gconf_client_add_dir (conf, CONF_GLOBAL_PREFIX,
			  GCONF_CLIENT_PRELOAD_ONELEVEL,
			  &err);
  if (err)
  {
    g_printerr ("There was an error loading config from %s. (%s)\n",
                  CONF_GLOBAL_PREFIX, err->message);
    g_error_free (err);
  }

  /* initialize the audio profiles part */
  gm_audio_profile_initialize (conf);

  /* register widgets */
  /* FIXME: add a comment why we need this at all, until then
     we comment it out
     gm_audio_profile_edit_get_type (); */
  glade_register_widget (gm_audio_profile_edit_get_type (),
			 NULL,
			 gtk_dialog_build_children,
			 dialog_find_internal_child);
  g_object_unref (G_OBJECT (conf));
}
