/* gmp-util.h: utility functions */

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
#include <gtk/gtk.h>
#include <glade/glade.h>

GladeXML*
gmp_util_load_glade_file (const char *filename,
                          const char *widget_root,
                          GtkWindow  *error_dialog_parent);
void
gmp_util_show_error_dialog (GtkWindow *transient_parent,
                            GtkWidget **weak_ptr,
                            const char *message_format, ...);

#ifdef DEBUG
#define GMP_DEBUG(...) G_STMT_START{ \
  { g_print ("DEBUG: "); g_print (__VA_ARGS__); } \
  } G_STMT_END
#else
#define GMP_DEBUG(...)
#endif
