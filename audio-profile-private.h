#ifndef AUDIO_PROFILE_PRIVATE_H
#define AUDIO_PROFILE_PRIVATE_H

#include "audio-profile.h"
#include <gconf/gconf-client.h>
#include "gmp-conf.h"
#include <glade/glade.h>
#include <glade/glade-build.h>

G_BEGIN_DECLS

GMAudioProfile*	gm_audio_profile_new		(const char *name,
                                                 GConfClient *conf);
char *          gm_audio_profile_create            (const char *name,
                                                 GConfClient *conf,
                                                 GError **error);

void		gm_audio_profile_initialize	(GConfClient *conf);
GList*		gm_audio_profile_get_list		(void);
int		gm_audio_profile_get_count		(void);
void		gm_audio_profile_forget		(GMAudioProfile *profile);
void		gm_audio_profile_sync_list         (gboolean use_this_list,
                                                 GSList  *this_list);


gboolean	gm_audio_setting_mask_is_empty	(const GMAudioSettingMask *mask);

void		gm_audio_profile_delete_list	(GConfClient *conf,
						 GList *deleted_profiles,
						 GError **error);
void        gtk_dialog_build_children (GladeXML *self, GtkWidget *w,
						GladeWidgetInfo *info);
GtkWidget*  dialog_find_internal_child (GladeXML *xml,
						GtkWidget *parent,
						const gchar *childname);

G_END_DECLS

#endif /* AUDIO_PROFILE_PRIVATE_H */
