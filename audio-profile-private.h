#ifndef AUDIO_PROFILE_PRIVATE_H
#define AUDIO_PROFILE_PRIVATE_H

#include <gconf/gconf-client.h>
#include "gmp-conf.h"

G_BEGIN_DECLS

/* mask for lockedness of settings */
typedef struct
{
  unsigned int name : 1;
  unsigned int description : 1;
  unsigned int pipeline : 1;
  unsigned int extension : 1;
  unsigned int active : 1;
} AudioSettingMask;

struct _AudioProfile
{
  GObject parent_instance;

  AudioProfilePrivate *priv;
};

struct _AudioProfileClass
{
  GObjectClass parent_class;

  void (* changed)   (AudioProfile           *profile,
                      const AudioSettingMask *mask);
  void (* forgotten) (AudioProfile           *profile);
};

GType audio_profile_get_type (void) G_GNUC_CONST;


AudioProfile*	audio_profile_new		(const char *name,
                                                 GConfClient *conf);
char *          audio_profile_create            (const char *name,
                                                 GConfClient *conf,
                                                 GError **error);

void		audio_profile_initialize	(GConfClient *conf);
GList*		audio_profile_get_list		(void);
int		audio_profile_get_count		(void);
void		audio_profile_forget		(AudioProfile *profile);
void		audio_profile_sync_list         (gboolean use_this_list,
                                                 GSList  *this_list);


gboolean	audio_setting_mask_is_empty	(const AudioSettingMask *mask);

void		audio_profile_delete_list	(GConfClient *conf,
						 GList *deleted_profiles,
						 GError **error);


#endif /* AUDIO_PROFILE_PRIVATE_H */
