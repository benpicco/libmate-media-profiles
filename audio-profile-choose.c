/* audio-profile-choose.c: combo box to choose a specific profile */

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
#include "audio-profile-choose.h"
#include "audio-profile.h"
#include <gtk/gtk.h>
#include <glade/glade-xml.h>

enum
{
  NAME_COLUMN,
  ID_COLUMN,
  N_COLUMNS
};

/* create and return a new Profile Choose combobox widget
 * given the GConf connection
 */

GtkWidget*
gm_audio_profile_choose_new ()
{
  GtkListStore *list_store;
  GtkTreeIter iter;
  GtkCellRenderer *renderer;
  GList *profiles = NULL;
  GtkWidget *combo;

  /* Create the model */
  list_store = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
  profiles = audio_profile_get_active_list ();
  while (profiles) {
    AudioProfile *profile = profiles->data;
    gtk_list_store_append (list_store, &iter);
    gtk_list_store_set (list_store, &iter,
                        NAME_COLUMN, audio_profile_get_name (profile),
                        ID_COLUMN, audio_profile_get_id (profile),
                        -1);
    profiles = profiles->next;
  }
  combo = gtk_combo_box_new_with_model (GTK_TREE_MODEL (list_store));

  /* display name in the combobox */
  renderer = gtk_cell_renderer_text_new ();
  gtk_cell_layout_pack_start (GTK_CELL_LAYOUT (combo),
                              renderer,
                              TRUE);
  gtk_cell_layout_set_attributes (GTK_CELL_LAYOUT (combo), renderer,
                                  "text", NAME_COLUMN,
                                  NULL);

  /* activate first one */
  gtk_combo_box_set_active (combo, 0);
  return combo;
}

/* get the currently active audio profile */
AudioProfile*
gm_audio_profile_choose_get_active (GtkWidget *choose)
{
  GtkTreeIter iter;
  GtkComboBox *combo = GTK_COMBO_BOX (choose);
  gchar *id;

  g_return_if_fail (GTK_IS_COMBO_BOX (choose));
  /* get active id */
  gtk_combo_box_get_active_iter (combo, &iter);
  gtk_tree_model_get (gtk_combo_box_get_model (combo), &iter,
                      ID_COLUMN, &id, -1);

  /* look up audio profile */
  return audio_profile_lookup (id);
}
