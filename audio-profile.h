#ifndef AUDIO_PROFILE_H
#define AUDIO_PROFILE_H

#include <gconf/gconf-client.h>
#include "gmp-conf.h"

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

GType audio_profile_get_type (void) G_GNUC_CONST;


const char*     audio_profile_get_id	        (AudioProfile *profile);
const char*     audio_profile_get_name	        (AudioProfile *profile);
const char*     audio_profile_get_description   (AudioProfile *profile);
const char*     audio_profile_get_pipeline	(AudioProfile *profile);
const char*     audio_profile_get_extension	(AudioProfile *profile);

GList*		audio_profile_get_active_list	(void);
AudioProfile*	audio_profile_lookup		(const char *id);

#endif /* AUDIO_PROFILE_H */
