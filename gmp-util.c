/* gmp-util.c: utility functions */

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

#include <glib/gi18n.h>
#include <gtk/gtk.h>
#include <glade/glade-xml.h>

#include "gmp-util.h"

GladeXML*
gmp_util_load_glade_file (const char *filename,
                          const char *widget_root,
                          GtkWindow  *error_dialog_parent)
{
  char *path;
  GladeXML *xml;

  xml = NULL;
  path = g_strconcat ("./", filename, NULL);

  if (g_file_test (path,
                   G_FILE_TEST_EXISTS))
    {
      /* Try current dir, for debugging */
      xml = glade_xml_new (path,
                           widget_root,
                           GETTEXT_PACKAGE);
    }

  if (xml == NULL)
    {
      g_free (path);

      path = g_build_filename (GMP_GLADE_DIR, filename, NULL);

      xml = glade_xml_new (path,
                           widget_root,
                           GETTEXT_PACKAGE);
    }

  if (xml == NULL)
    {
      static GtkWidget *no_glade_dialog = NULL;

      gmp_util_show_error_dialog (error_dialog_parent, &no_glade_dialog,
                                       _("The file \"%s\" is missing. This indicates that the application is installed incorrectly, so the dialog can't be displayed."), path);
    }

  g_free (path);

  return xml;
}

void
gmp_util_run_error_dialog (GtkWindow *transient_parent, const char *message_format, ...)
{
  char *message;
  va_list args;
  GtkWidget *dialog;

  if (message_format)
  {
    va_start (args, message_format);
    message = g_strdup_vprintf (message_format, args);
    va_end (args);
  }
  else message = NULL;

  dialog = gtk_message_dialog_new (transient_parent,
                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                 GTK_MESSAGE_ERROR,
                                 GTK_BUTTONS_CLOSE,
                                 "%s",
                                 message);

  gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
  gtk_dialog_run (GTK_DIALOG (dialog));
  gtk_widget_destroy(GTK_WIDGET (dialog));
}

/**
 * gmp_util_show_error_dialog:
 * @transient_parent: parent of the future dialog window;
 * @weap_ptr: pointer to a #Widget pointer, to control the population.
 * @message_format: printf() style format string
 *
 * Create a #GtkMessageDialog window with the message, and present it,
 * handling its buttons.
 * If @weap_ptr is not #NULL, only create the dialog if
 * <literal>*weap_ptr</literal> is #NULL
 * (and in that case, set @weap_ptr to be a weak pointer to the new dialog),
 * otherwise just present <literal>*weak_ptr</literal>. Note that in this
 * last case, the message <emph>will</emph>  be changed.
 */

void
gmp_util_show_error_dialog (GtkWindow *transient_parent,
                            GtkWidget **weak_ptr,
                            const char *message_format, ...)
{
  char *message;
  va_list args;

  if (message_format)
  {
    va_start (args, message_format);
    message = g_strdup_vprintf (message_format, args);
    va_end (args);
  }
  else message = NULL;

  if (weak_ptr == NULL || *weak_ptr == NULL)
  {
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new (transient_parent,
                                     GTK_DIALOG_DESTROY_WITH_PARENT,
                                     GTK_MESSAGE_ERROR,
                                     GTK_BUTTONS_CLOSE,
                                     "%s",
                                     message);

    g_signal_connect (G_OBJECT (dialog), "response",
                      G_CALLBACK (gtk_widget_destroy), NULL);

    if (weak_ptr != NULL)
    {
      *weak_ptr = dialog;
      g_object_add_weak_pointer (G_OBJECT (dialog), (void**)weak_ptr);
    }

    gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

    gtk_widget_show_all (dialog);
  }
  else
  {
    g_return_if_fail (GTK_IS_MESSAGE_DIALOG (*weak_ptr));

    gtk_label_set_text (GTK_LABEL (GTK_MESSAGE_DIALOG (*weak_ptr)->label),
                        message);

    gtk_window_present (GTK_WINDOW (*weak_ptr));
  }
}
