#ifndef AUDIO_PROFILE_H
#define AUDIO_PROFILE_H

#include <gconf/gconf-client.h>
#include "gmp-conf.h"

/* mask for lockedness of settings */
typedef struct
{
  unsigned int name : 1;
  unsigned int description : 1;
  unsigned int pipeline : 1;
  unsigned int extension : 1;
  unsigned int active : 1;
} AudioSettingMask;

G_BEGIN_DECLS

#define AUDIO_TYPE_PROFILE              (audio_profile_get_type ())
#define AUDIO_PROFILE(object)           (G_TYPE_CHECK_INSTANCE_CAST ((object), AUDIO_TYPE_PROFILE, AudioProfile))
#define AUDIO_PROFILE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST ((klass), AUDIO_TYPE_PROFILE, AudioProfileClass))
#define AUDIO_IS_PROFILE(object)        (G_TYPE_CHECK_INSTANCE_TYPE ((object), AUDIO_TYPE_PROFILE))
#define AUDIO_IS_PROFILE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), AUDIO_TYPE_PROFILE))
#define AUDIO_PROFILE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS ((obj), AUDIO_TYPE_PROFILE, AudioProfileClass))

typedef struct _AudioProfile        AudioProfile;
typedef struct _AudioProfileClass   AudioProfileClass;
typedef struct _AudioProfilePrivate AudioProfilePrivate;

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

const char*     audio_profile_get_id	        (AudioProfile *profile);
const char*     audio_profile_get_name	        (AudioProfile *profile);
const char*     audio_profile_get_description   (AudioProfile *profile);
const char*     audio_profile_get_pipeline	(AudioProfile *profile);
const char*     audio_profile_get_extension	(AudioProfile *profile);

void		audio_profile_initialize	(GConfClient *conf);
GList*		audio_profile_get_list		(void);
GList*		audio_profile_get_active_list	(void);
int		audio_profile_get_count		(void);
AudioProfile*	audio_profile_lookup		(const char *name);
void		audio_profile_forget		(AudioProfile *profile);
void		audio_profile_sync_list         (gboolean use_this_list,
                                                 GSList  *this_list);


gboolean	audio_setting_mask_is_empty	(const AudioSettingMask *mask);

void		audio_profile_delete_list	(GConfClient *conf,
						 GList *deleted_profiles,
						 GError **error);


#endif /* AUDIO_PROFILE_H */
