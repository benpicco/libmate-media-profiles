/* gm_audio-profile-choose.c: combo box to choose a specific profile */

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

#include <string.h>
#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include <gst/gst.h>

#include "gmp-util.h"
#include "audio-profile-choose.h"
#include "audio-profile.h"

enum
{
  NAME_COLUMN,
  ID_COLUMN,
  N_COLUMNS
};

static void
audio_profile_forgotten (GMAudioProfile *profile, GtkWidget *combo)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  char *tmp;
  const char *id;

  g_return_if_fail (GTK_IS_COMBO_BOX (combo));
  g_return_if_fail (GM_AUDIO_PROFILE (profile));

  id = gm_audio_profile_get_id (profile);
  GST_DEBUG ("forgotten id: %s", id);

  model = gtk_combo_box_get_model (GTK_COMBO_BOX (combo));

  if (!gtk_tree_model_get_iter_first (model, &iter))
    return;

  while (1)
    {
      gtk_tree_model_get (model, &iter, ID_COLUMN, &tmp, -1);
      if (g_str_equal (tmp, id))
	{
	  gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
	  g_free (tmp);
	  return;
	}
      g_free (tmp);

      if (!gtk_tree_model_iter_next (model, &iter))
	break;
    }
}

/* create and return a new Profile Choose combobox widget
 * given the GConf connection
 */

GtkWidget*
gm_audio_profile_choose_new (void)
{
  GtkListStore *list_store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GList *profiles, *orig;
  GtkWidget *combo;

  /* Create the model */
  list_store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
  orig = profiles = gm_audio_profile_get_active_list ();
  combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

  while (profiles) {
    GMAudioProfile *profile = profiles->data;
    char *profile_name, *temp_file_name;
    char* mime_type_description;

    temp_file_name = g_strdup_printf (".%s", gm_audio_profile_get_extension (profile));
    mime_type_description = g_content_type_get_description (temp_file_name);
    g_free (temp_file_name);

    profile_name = g_strdup_printf (gettext ("%s (%s)"), gm_audio_profile_get_name (profile), mime_type_description);
    g_free (mime_type_description);
    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                        NAME_COLUMN, profile_name,
                        ID_COLUMN, gm_audio_profile_get_id (profile),
                        -1);

    g_signal_connect (profile, "forgotten", G_CALLBACK (audio_profile_forgotten), combo);

    profiles = profiles->next;
    g_free (profile_name);
  }
  g_list_free (orig);

  /* display name in the combobox */
  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo),
                              renderer,
                              TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                  "text", NAME_COLUMN,
                                  NULL);

  /* activate first one */
  gtk_combo_box_set_active (GTK_COMBO_BOX (combo), 0);

  return combo;
}

/* get the currently active gm_audio profile */
GMAudioProfile*
gm_audio_profile_choose_get_active (GtkWidget *choose)
{
  GtkTreeIter iter;
  GtkComboBox *combo = GTK_COMBO_BOX (choose);
  gchar *id;
  GMAudioProfile *profile = NULL;
  
  g_return_val_if_fail (GTK_IS_COMBO_BOX (choose), NULL);
  /* get active id */
  if (gtk_combo_box_get_active_iter (combo, &iter)) {
    gtk_tree_model_get (gtk_combo_box_get_model (combo), &iter,
                        ID_COLUMN, &id, -1);
    
    /* look up gm_audio profile */
    profile = gm_audio_profile_lookup (id);
    g_free (id);
  }
  return profile;
}

gboolean
gm_audio_profile_choose_set_active (GtkWidget  *choose,
				    const char *id)
{
  GtkTreeModel *model;
  GtkTreeIter iter;
  char *tmp;
  
  g_return_val_if_fail (GTK_IS_COMBO_BOX (choose), FALSE);

  model = gtk_combo_box_get_model (GTK_COMBO_BOX (choose));
  
  if (!gtk_tree_model_get_iter_first (model, &iter))
    return FALSE;

  while (1)
    {
      gtk_tree_model_get (model, &iter, ID_COLUMN, &tmp, -1);
      if (!strcmp (tmp, id))
	{
	  gtk_combo_box_set_active_iter (GTK_COMBO_BOX (choose), &iter);
	  g_free (tmp);
	  return TRUE;
	}
      g_free (tmp);      
      
      if (!gtk_tree_model_iter_next (model, &iter))
	break;
    }

  /* Fallback to first entry */
  gtk_tree_model_get_iter_first (model, &iter);
  gtk_combo_box_set_active_iter (GTK_COMBO_BOX (choose), &iter);
  
  return FALSE;
}
