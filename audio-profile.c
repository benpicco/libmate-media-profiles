#include <string.h>

#include "libgnome/gnome-i18n.h"
#include "gmp-util.h"
#include "audio-profile.h"
#include "audio-profile-private.h"

#define KEY_NAME "name"
#define KEY_DESCRIPTION "description"
#define KEY_PIPELINE "pipeline"
#define KEY_EXTENSION "extension"
#define KEY_ACTIVE "active"

struct _GMAudioProfilePrivate
{
  char *id;		     /* the GConf dir name */
  char *profile_dir;         /* full path in GConf to this profile */
  GConfClient *conf;
  guint notify_id;

  int in_notification_count; /* don't understand, see terminal-profile.c */
  char *name;                /* human-readable short name */
  char *description;         /* longer description of profile */
  char *pipeline;            /* GStreamer pipeline to be used */
  char *extension;           /* default file extension for this format */
  guint active : 1;
  guint forgotten : 1;

  GMAudioSettingMask locked;
};

static GHashTable *profiles = NULL;
static GConfClient *_conf = NULL;

#define RETURN_IF_NOTIFYING(profile) if ((profile)->priv->in_notification_count) return

enum {
  CHANGED,
  FORGOTTEN,
  LAST_SIGNAL
};

static void gm_audio_profile_init        (GMAudioProfile      *profile);
static void gm_audio_profile_class_init  (GMAudioProfileClass *klass);
static void gm_audio_profile_finalize    (GObject              *object);

static void gm_audio_profile_update      (GMAudioProfile *profile);

static void profile_change_notify     (GConfClient *client,
                                       guint        cnxn_id,
                                       GConfEntry  *entry,
                                       gpointer     user_data);

static void emit_changed (GMAudioProfile           *profile,
                          const GMAudioSettingMask *mask);


static gpointer parent_class;
static guint signals[LAST_SIGNAL] = { 0 };


static gpointer parent_class;

/*
 * GObject stuff
 */

GType
gm_audio_profile_get_type (void)
{
  static GType object_type = 0;

  g_type_init ();

  if (!object_type)
    {
      static const GTypeInfo object_info =
      {
        sizeof (GMAudioProfileClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        (GClassInitFunc) gm_audio_profile_class_init,
        NULL,           /* class_finalize */
        NULL,           /* class_data */
        sizeof (GMAudioProfile),
        0,              /* n_preallocs */
        (GInstanceInitFunc) gm_audio_profile_init,
      };
      object_type = g_type_register_static (G_TYPE_OBJECT,
                                            "GMAudioProfile",
                                            &object_info, 0);
    }

  return object_type;
}

static void
gm_audio_profile_init (GMAudioProfile *profile)
{
  g_return_if_fail (profiles != NULL);

  profile->priv = g_new0 (GMAudioProfilePrivate, 1);
  profile->priv->name = g_strdup (_("<not named>"));
  profile->priv->description = g_strdup (_("<not described>"));
  profile->priv->pipeline = g_strdup ("identity");
  profile->priv->extension = g_strdup ("wav");
}

static void
gm_audio_profile_class_init (GMAudioProfileClass *klass)
{
  GObjectClass *object_class = G_OBJECT_CLASS (klass);

  parent_class = g_type_class_peek_parent (klass);

  object_class->finalize = gm_audio_profile_finalize;

  signals[CHANGED] =
    g_signal_new ("changed",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GMAudioProfileClass, changed),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER,
                  G_TYPE_NONE, 1, G_TYPE_POINTER);

  signals[FORGOTTEN] =
    g_signal_new ("forgotten",
                  G_OBJECT_CLASS_TYPE (object_class),
                  G_SIGNAL_RUN_LAST,
                  G_STRUCT_OFFSET (GMAudioProfileClass, forgotten),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__VOID,
                  G_TYPE_NONE, 0);
}

static void
gm_audio_profile_finalize (GObject *object)
{
  GMAudioProfile *profile;

  profile = GM_AUDIO_PROFILE (object);

  gm_audio_profile_forget (profile);

  gconf_client_notify_remove (profile->priv->conf,
                              profile->priv->notify_id);
  profile->priv->notify_id = 0;

  g_object_unref (G_OBJECT (profile->priv->conf));

  g_free (profile->priv->name);

  G_OBJECT_CLASS (parent_class)->finalize (object);
}

/*
 * internal stuff to manage profiles
 */

/* sync gm_audio profiles list by either using the given list as the new list
 * or by getting the list from GConf
 */

static GList*
find_profile_link (GList      *profiles,
                   const char *id)
{
  GList *tmp;

  tmp = profiles;
  while (tmp != NULL)
    {
      if (strcmp (gm_audio_profile_get_id (GM_AUDIO_PROFILE (tmp->data)),
                  id) == 0)
        return tmp;

      tmp = tmp->next;
    }

  return NULL;
}

/* synchronize global profiles hash through accessor functions
 * if use_this_list is true, then put given profiles to the hash
 * if it's false, then get list from GConf */
void
gm_audio_profile_sync_list (gboolean use_this_list,
                         GSList  *this_list)
{
  GList *known;
  GSList *updated;
  GList *tmp_list;
  GSList *tmp_slist;
  GError *err;
  gboolean need_new_default;
  GMAudioProfile *fallback;

  GMP_DEBUG("sync_list: start\n");
  if (use_this_list)
    GMP_DEBUG("Using given list of length %d\n", g_slist_length (this_list));
  else
    GMP_DEBUG("using list from gconf\n");
  known = gm_audio_profile_get_list ();
    GMP_DEBUG("list of known profiles: size %d\n", g_list_length (known));

  if (use_this_list)
    {
      updated = g_slist_copy (this_list);
    }
  else
    {
      err = NULL;
      updated = gconf_client_get_list (_conf,
                                       CONF_GLOBAL_PREFIX"/profile_list",
                                       GCONF_VALUE_STRING,
                                       &err);
      if (err)
        {           g_printerr (_("There was an error getting the list of gm_audio profiles. (%s)\n"),
                      err->message);
          g_error_free (err);
        }
    }

  GMP_DEBUG("updated: slist of %d items\n", g_slist_length (updated));
  /* Add any new ones; ie go through updated and if any of them isn't in
   * the hash yet, add it.  If it is in the list of known profiles,  remove
   * it from our copy of that list. */
  tmp_slist = updated;
  while (tmp_slist != NULL)
    {
      GList *link;

      link = find_profile_link (known, tmp_slist->data);

      if (link)
        {
          /* make known point to profiles we didn't find in the list */
          GMP_DEBUG("id %s found in known profiles list, deleting from known\n",
                    (char *) tmp_slist->data);
          known = g_list_delete_link (known, link);
        }
      else
        {
          GMAudioProfile *profile;

          GMP_DEBUG("adding new profile with id %s to global hash\n",
                   (const char *) tmp_slist->data);
          profile = gm_audio_profile_new (tmp_slist->data, _conf);

          gm_audio_profile_update (profile);
        }

      if (!use_this_list)
        g_free (tmp_slist->data);

      tmp_slist = tmp_slist->next;
    }

  g_slist_free (updated);

  fallback = NULL;

    /* Forget no-longer-existing profiles */
  need_new_default = FALSE;
  tmp_list = known;
  while (tmp_list != NULL)
    {
      GMAudioProfile *forgotten;

      forgotten = GM_AUDIO_PROFILE (tmp_list->data);

      GMP_DEBUG("sync_list: forgetting profile with id %s\n",
               gm_audio_profile_get_id (forgotten));
      gm_audio_profile_forget (forgotten);

      tmp_list = tmp_list->next;
    }

  g_list_free (known);
  GMP_DEBUG("sync_list: stop\n");

  //FIXME: g_assert (terminal_profile_get_count () > 0);
}

/*
 * external API functions
 */

/* create a new GMAudioProfile structure and add it to the global profiles hash
 * load settings from GConf tree
 */
GMAudioProfile*
gm_audio_profile_new (const char *id, GConfClient *conf)
{
  GMAudioProfile *profile;
  GError *err;

  GMP_DEBUG("creating new GMAudioProfile for id %s\n", id);
  g_return_val_if_fail (profiles != NULL, NULL);
  g_return_val_if_fail (gm_audio_profile_lookup (id) == NULL, NULL);

  profile = g_object_new (GM_AUDIO_TYPE_PROFILE, NULL);

  profile->priv->conf = conf;
  g_object_ref (G_OBJECT (conf));

  profile->priv->id = g_strdup (id);
  profile->priv->profile_dir = gconf_concat_dir_and_key (CONF_PROFILES_PREFIX,
                                                         profile->priv->id);

  err = NULL;
  GMP_DEBUG("loading config from GConf dir %s\n",
           profile->priv->profile_dir);
  gconf_client_add_dir (conf, profile->priv->profile_dir,
                        GCONF_CLIENT_PRELOAD_ONELEVEL,
                        &err);
  if (err)
    {
      g_printerr ("There was an error loading config from %s. (%s)\n",
                    profile->priv->profile_dir, err->message);
      g_error_free (err);
    }

  err = NULL;
  GMP_DEBUG("adding notify for GConf profile\n");
  profile->priv->notify_id =
    gconf_client_notify_add (conf,
                             profile->priv->profile_dir,
                             profile_change_notify,
                             profile,
                             NULL, &err);

  if (err)
    {
      g_printerr ("There was an error subscribing to notification of gm_audio profile changes. (%s)\n",
                  err->message);
      g_error_free (err);
    }

  GMP_DEBUG("inserting in hash table done\n");
  g_hash_table_insert (profiles, profile->priv->id, profile);
  GMP_DEBUG("audio_profile_new done\n");

  return profile;
}

/*
 * public profile getters and setters
 */

const char*
gm_audio_profile_get_id (GMAudioProfile *profile)
{
  return profile->priv->id;
}

const char*
gm_audio_profile_get_name (GMAudioProfile *profile)
{
  return profile->priv->name;
}

void
gm_audio_profile_set_name (GMAudioProfile *profile,
                        const char      *name)
{
  char *key;

  RETURN_IF_NOTIFYING (profile);

  key = gconf_concat_dir_and_key (profile->priv->profile_dir,
                                  KEY_NAME);

  gconf_client_set_string (profile->priv->conf,
                           key,
                           name,
                           NULL);

  g_free (key);
}

const char*
gm_audio_profile_get_description (GMAudioProfile *profile)
{
  return profile->priv->description;
}

void
gm_audio_profile_set_description (GMAudioProfile *profile,
                               const char   *description)
{
  char *key;

  RETURN_IF_NOTIFYING (profile);

  key = gconf_concat_dir_and_key (profile->priv->profile_dir,
                                  KEY_DESCRIPTION);

  gconf_client_set_string (profile->priv->conf,
                           key,
                           description,
                           NULL);

  g_free (key);
}

const char*
gm_audio_profile_get_pipeline (GMAudioProfile *profile)
{
  return profile->priv->pipeline;
}

void
gm_audio_profile_set_pipeline (GMAudioProfile *profile,
                            const char   *pipeline)
{
  char *key;

  RETURN_IF_NOTIFYING (profile);

  key = gconf_concat_dir_and_key (profile->priv->profile_dir,
                                  KEY_PIPELINE);

  gconf_client_set_string (profile->priv->conf,
                           key,
                           pipeline,
                           NULL);

  g_free (key);
}

const char*
gm_audio_profile_get_extension (GMAudioProfile *profile)
{
  return profile->priv->extension;
}

void
gm_audio_profile_set_extension (GMAudioProfile *profile,
                               const char   *extension)
{
  char *key;

  RETURN_IF_NOTIFYING (profile);

  key = gconf_concat_dir_and_key (profile->priv->profile_dir,
                                  KEY_EXTENSION);

  gconf_client_set_string (profile->priv->conf,
                           key,
                           extension,
                           NULL);

  g_free (key);
}

gboolean
gm_audio_profile_get_active (GMAudioProfile *profile)
{
  return profile->priv->active;
}

void
gm_audio_profile_set_active (GMAudioProfile *profile,
                          gboolean active)
{
  char *key;

  RETURN_IF_NOTIFYING (profile);

  key = gconf_concat_dir_and_key (profile->priv->profile_dir,
                                  KEY_ACTIVE);

  gconf_client_set_bool (profile->priv->conf,
                         key,
                         active,
                         NULL);

  g_free (key);
}
/*
 * private setters
 */

static gboolean
set_name (GMAudioProfile *profile,
          const char *candidate_name)
{
  /* don't update if it's the same as the old one */
  if (candidate_name &&
      strcmp (profile->priv->name, candidate_name) == 0)
    return FALSE;

  if (candidate_name != NULL)
    {
      g_free (profile->priv->name);
      profile->priv->name = g_strdup (candidate_name);
      return TRUE;
    }
  /* otherwise just leave the old name */

  return FALSE;
}

static gboolean
set_description (GMAudioProfile *profile,
                 const char *candidate_description)
{
  /* don't update if it's the same as the old one */
  if (candidate_description &&
      strcmp (profile->priv->description, candidate_description) == 0)
    return FALSE;

  if (candidate_description != NULL)
    {
      g_free (profile->priv->description);
      profile->priv->description = g_strdup (candidate_description);
      return TRUE;
    }
  /* otherwise just leave the old description */

  return FALSE;
}

static gboolean
set_pipeline (GMAudioProfile *profile,
              const char *candidate_pipeline)
{
  /* don't update if it's the same as the old one */
  if (candidate_pipeline &&
      strcmp (profile->priv->pipeline, candidate_pipeline) == 0)
    return FALSE;

  if (candidate_pipeline != NULL)
    {
      g_free (profile->priv->pipeline);
      profile->priv->pipeline = g_strdup (candidate_pipeline);
      return TRUE;
    }
  /* otherwise just leave the old pipeline */

  return FALSE;
}

static gboolean
set_extension (GMAudioProfile *profile,
               const char *candidate_extension)
{
  /* don't update if it's the same as the old one */
  if (candidate_extension &&
      strcmp (profile->priv->extension, candidate_extension) == 0)
    return FALSE;

  if (candidate_extension != NULL)
    {
      g_free (profile->priv->extension);
      profile->priv->extension = g_strdup (candidate_extension);
      return TRUE;
    }
  /* otherwise just leave the old extension */

  return FALSE;
}

static const gchar*
find_key (const gchar* key)
{
  const gchar* end;

  end = strrchr (key, '/');

  ++end;

  return end;
}

static void
profile_change_notify (GConfClient *client,
                       guint        cnxn_id,
                       GConfEntry  *entry,
                       gpointer     user_data)
{
  GMAudioProfile *profile;
  const char *key;
  GConfValue *val;
  GMAudioSettingMask mask; /* to keep track of what has changed */

  profile = GM_AUDIO_PROFILE (user_data);
  GMP_DEBUG("profile_change_notify: start in profile with name %s\n",
           profile->priv->name);

  val = gconf_entry_get_value (entry);

  key = find_key (gconf_entry_get_key (entry));

/* strings are set through static set_ functions */
#define UPDATE_STRING(KName, FName, Preset)                             \
  }                                                                     \
else if (strcmp (key, KName) == 0)                                      \
  {                                                                     \
    const char * setting = (Preset);                                    \
                                                                        \
    if (val && val->type == GCONF_VALUE_STRING)                         \
      setting = gconf_value_get_string (val);                           \
                                                                        \
    mask.FName = set_##FName (profile, setting);                        \
                                                                        \
    profile->priv->locked.FName = !gconf_entry_get_is_writable (entry);

/* booleans are set directly on the profile priv variable */
#define UPDATE_BOOLEAN(KName, FName, Preset)                            \
  }                                                                     \
else if (strcmp (key, KName) == 0)                                      \
  {                                                                     \
    gboolean setting = (Preset);                                        \
                                                                        \
    if (val && val->type == GCONF_VALUE_BOOL)                           \
      setting = gconf_value_get_bool (val);                             \
                                                                        \
    if (setting != profile->priv->FName)                                \
      {                                                                 \
        mask.FName = TRUE;                                              \
        profile->priv->FName = setting;                                 \
      }                                                                 \
                                                                        \
    profile->priv->locked.FName = !gconf_entry_get_is_writable (entry);

  if (0)
  {
    UPDATE_STRING (KEY_NAME,        name, NULL);
    UPDATE_STRING (KEY_DESCRIPTION, description, NULL);
    UPDATE_STRING (KEY_PIPELINE,    pipeline, NULL);
    UPDATE_STRING (KEY_EXTENSION,   extension, NULL);
    UPDATE_BOOLEAN (KEY_ACTIVE, active, TRUE);
  }

#undef UPDATE_STRING
#undef UPDATE_BOOLEAN

  if (!(gm_audio_setting_mask_is_empty (&mask)))
  {
    GMP_DEBUG("emit changed\n");
    emit_changed (profile, &mask);
  }
  GMP_DEBUG("PROFILE_CHANGE_NOTIFY: changed stuff\n");
}

/* GConf notification callback for profile_list */
static void
gm_audio_profile_list_notify (GConfClient *client,
                              guint        cnxn_id,
                              GConfEntry  *entry,
                              gpointer     user_data)
{
  GConfValue *val;
  GSList *value_list;
  GSList *string_list;
  GSList *tmp;

  GMP_DEBUG("profile_list changed\n");
  val = gconf_entry_get_value (entry);

  if (val == NULL ||
      val->type != GCONF_VALUE_LIST ||
      gconf_value_get_list_type (val) != GCONF_VALUE_STRING)
    value_list = NULL;
  else
    value_list = gconf_value_get_list (val);

  string_list = NULL;
  tmp = value_list;
  while (tmp != NULL)
    {
      string_list = g_slist_prepend (string_list,
                                     g_strdup (gconf_value_get_string ((GConfValue*) tmp->data)));

      tmp = tmp->next;
    }

  string_list = g_slist_reverse (string_list);

  gm_audio_profile_sync_list (TRUE, string_list);

  g_slist_foreach (string_list, (GFunc) g_free, NULL);
  g_slist_free (string_list);
}


/* needs to be called once
 * sets up the global profiles hash
 * safe to call more than once
 */
void
gm_audio_profile_initialize (GConfClient *conf)
{
  GError *err;
/*
  char *str;
*/

  g_return_if_fail (profiles == NULL);

  profiles = g_hash_table_new (g_str_hash, g_str_equal);

  if (_conf == NULL) _conf = conf;
  /* sync it for the first time */
  gm_audio_profile_sync_list (FALSE, NULL);

  /* subscribe to changes to profile list */
  err = NULL;
  gconf_client_notify_add (conf,
                           CONF_GLOBAL_PREFIX"/profile_list",
                           gm_audio_profile_list_notify,
                           NULL,
                           NULL, &err);

  if (err)
    {
      g_printerr (_("There was an error subscribing to notification of audio profile list changes. (%s)\n"),
                  err->message);
      g_error_free (err);
    }


  /* FIXME: no defaults
  err = NULL;
  gconf_client_notify_add (conf,
                           CONF_GLOBAL_PREFIX"/default_profile",                            default_change_notify,
                           NULL,
                           NULL, &err);
  if (err)
    {
      g_printerr (_("There was an error subscribing to notification of changes to default profile. (%s)\n"),
                  err->message);
      g_error_free (err);
    }

  str = gconf_client_get_string (conf,
                                 CONF_GLOBAL_PREFIX"/default_profile",
                                 NULL);
  if (str)
    {
      update_default_profile (str,
                              !gconf_client_key_is_writable (conf,
                                                             CONF_GLOBAL_PREFIX"/default_profile",
                                                             NULL));
      g_free (str);
    }
  */
}

static void
emit_changed (GMAudioProfile           *profile,
              const GMAudioSettingMask *mask)
{
  profile->priv->in_notification_count += 1;
  g_signal_emit (G_OBJECT (profile), signals[CHANGED], 0, mask);
  profile->priv->in_notification_count -= 1;
}


/* update the given GMAudioProfile from GConf */
static void
gm_audio_profile_update (GMAudioProfile *profile)
{
  GMAudioSettingMask locked;
  GMAudioSettingMask mask;

  memset (&mask, '\0', sizeof (mask));
  memset (&locked, '\0', sizeof (locked));

#define UPDATE_BOOLEAN(KName, FName) \
{ \
  char *key = gconf_concat_dir_and_key (profile->priv->profile_dir, KName); \
  gboolean val = gconf_client_get_bool (profile->priv->conf, key, NULL);    \
                                                                            \
  if (val != profile->priv->FName) \
  { \
    mask.FName = TRUE; \
    profile->priv->FName = val; \
  } \
                                                                              \
  locked.FName = \
    !gconf_client_key_is_writable (profile->priv->conf, key, NULL); \
                                                                              \
  g_free (key); \
}
#define UPDATE_STRING(KName, FName) \
{ \
  char *key = gconf_concat_dir_and_key (profile->priv->profile_dir, KName); \
  char *val = gconf_client_get_string (profile->priv->conf, key, NULL); \
                                                                               \
  mask.FName = set_##FName (profile, val); \
                                                                               \
  locked.FName = \
    !gconf_client_key_is_writable (profile->priv->conf, key, NULL); \
                                                                               \
  g_free (val); \
  g_free (key); \
}
  UPDATE_STRING  (KEY_NAME,        name);
  UPDATE_STRING  (KEY_DESCRIPTION, description);
  UPDATE_STRING  (KEY_PIPELINE,    pipeline);
  UPDATE_STRING  (KEY_EXTENSION,   extension);
  UPDATE_BOOLEAN (KEY_ACTIVE,      active);

#undef UPDATE_BOOLEAN
#undef UPDATE_STRING
  profile->priv->locked = locked;
  //FIXME: we don't use mask ?
}


static void
listify_foreach (gpointer key,
                 gpointer value,
                 gpointer data)
{
  GList **listp = data;

  *listp = g_list_prepend (*listp, value);
}

static int
alphabetic_cmp (gconstpointer a,
                gconstpointer b)
{
  GMAudioProfile *ap = (GMAudioProfile*) a;
  GMAudioProfile *bp = (GMAudioProfile*) b;

  return g_utf8_collate (gm_audio_profile_get_name (ap),
                         gm_audio_profile_get_name (bp));
}

GList*
gm_audio_profile_get_list (void)
{
  GList *list;

  list = NULL;
  g_hash_table_foreach (profiles, listify_foreach, &list);

  list = g_list_sort (list, alphabetic_cmp);

  return list;
}

/* Return a GList of active GMAudioProfile's only */
GList*
gm_audio_profile_get_active_list (void)
{
  GList *list;
  GList *new_list;

  list = gm_audio_profile_get_list ();

  new_list = NULL;
  while (list)
  {
    GMAudioProfile *profile;

    profile = (GMAudioProfile *) list->data;
    if (gm_audio_profile_get_active (profile))
      new_list = g_list_append (new_list, list->data);
    list = g_list_next (list);
  }

  return new_list;
}

int
gm_audio_profile_get_count (void)
{
  return g_hash_table_size (profiles);
}

GMAudioProfile*
gm_audio_profile_lookup (const char *id)
{
  g_return_val_if_fail (id != NULL, NULL);

  if (profiles)
  {
    GMP_DEBUG("a_p_l: profiles exists, returning hash table lookup of %s\n", id);
    return g_hash_table_lookup (profiles, id);
  }
  else
    return NULL;
}

void
gm_audio_profile_forget (GMAudioProfile *profile)
{
  GMP_DEBUG("audio_profile_forget: forgetting name %s\n",
           gm_audio_profile_get_name (profile));
  if (!profile->priv->forgotten)
  {
    GError *err;

    err = NULL;
    GMP_DEBUG("audio_profile_forget: removing from gconf\n");
    /* FIXME: remove_dir doesn't actually work.  Either unset all keys
     * manually or use recursive_unset on HEAD */
    gconf_client_remove_dir (profile->priv->conf,
                             profile->priv->profile_dir,
                             &err);
    if (err)
    {
      g_printerr (_("There was an error forgetting profile path %s. (%s)\n"),
                  profile->priv->profile_dir, err->message);
                  g_error_free (err);
    }

    g_hash_table_remove (profiles, profile->priv->name);
    profile->priv->forgotten = TRUE;

    g_signal_emit (G_OBJECT (profile), signals[FORGOTTEN], 0);
  }
  else
    GMP_DEBUG("audio_profile_forget: profile->priv->forgotten\n");
}

gboolean
gm_audio_setting_mask_is_empty (const GMAudioSettingMask *mask)
{
  const unsigned int *p = (const unsigned int *) mask;
  const unsigned int *end = p + (sizeof (GMAudioSettingMask) /
                                 sizeof (unsigned int));

  while (p < end)
  {
    if (*p != 0)
      return FALSE;
    ++p;
  }

  return TRUE;
}

/* gm_audio_profile_create returns the unique id of the created profile,
 * which is used for looking up profiles later on.
 * Caller should free the returned id */
char *
gm_audio_profile_create (const char  *name,
                      GConfClient *conf,
                      GError      **error)
{
  char *profile_id = NULL;
  char *profile_dir = NULL;
  int i;
  char *s;
  char *key = NULL;
  GError *err = NULL;
  GList *profiles = NULL;
  GSList *id_list = NULL;
  GList *tmp;

  GMP_DEBUG("a_p_c: Creating profile for %s\n", name);
  /* This is for extra bonus paranoia against CORBA reentrancy */
  //g_object_ref (G_OBJECT (transient_parent));
#define BAIL_OUT_CHECK() do {                           \
      if (err != NULL)					\
       goto cleanup;                                    \
  } while (0)

  /* Pick a unique name for storing in gconf (based on visible name) */
  profile_id = gconf_escape_key (name, -1);
  s = g_strdup (profile_id);
  GMP_DEBUG("profile_id: %s\n", s);
  i = 0;
  while (gm_audio_profile_lookup (s))
  {
    g_free (s);
    s = g_strdup_printf ("%s-%d", profile_id, i);
    ++i;
  }
  g_free (profile_id);
  profile_id = s;
  g_free (s);

  profile_dir = gconf_concat_dir_and_key (CONF_PROFILES_PREFIX,
                                          profile_id);

  /* Store a copy of default profile values at under that directory */
  key = gconf_concat_dir_and_key (profile_dir,
                                  KEY_NAME);

  gconf_client_set_string (conf,
                           key,
                           name,
                           &err);
  BAIL_OUT_CHECK ();
  if (err != NULL) g_print ("ERROR: msg: %s\n", err->message);
  g_free (key);

  key = gconf_concat_dir_and_key (profile_dir,
                                  KEY_DESCRIPTION);

  gconf_client_set_string (conf,
                           key,
                           _("<no description>"),
                           &err);
  BAIL_OUT_CHECK ();
  if (err != NULL) g_print ("ERROR: msg: %s\n", err->message);
  g_free (key);

  key = gconf_concat_dir_and_key (profile_dir,
                                  KEY_PIPELINE);

  gconf_client_set_string (conf,
                           key,
                           _("identity"),
                           &err);
  BAIL_OUT_CHECK ();
  if (err != NULL) g_print ("ERROR: msg: %s\n", err->message);
  g_free (key);

  key = gconf_concat_dir_and_key (profile_dir,
                                  KEY_EXTENSION);

  gconf_client_set_string (conf,
                           key,
                           _("wav"),
                           &err);
  BAIL_OUT_CHECK ();
  if (err != NULL) g_print ("ERROR: msg: %s\n", err->message);

  /* Add new profile to the profile list; the method for doing this has
   * a race condition where we and someone else set at the same time,
   * but I am just going to punt on this issue.
   */
  profiles = gm_audio_profile_get_list ();
  tmp = profiles;
  while (tmp != NULL)
  {
    id_list = g_slist_prepend (id_list,
                               g_strdup (gm_audio_profile_get_id (tmp->data)));
    tmp = tmp->next;
  }

  id_list = g_slist_prepend (id_list, g_strdup (profile_id));

  GMP_DEBUG("setting gconf list\n");
  err = NULL;
  gconf_client_set_list (conf,
                         CONF_GLOBAL_PREFIX"/profile_list",
                         GCONF_VALUE_STRING,
                         id_list,
                         &err);

  BAIL_OUT_CHECK ();

 cleanup:
  /* run both when being dumped here through errors and normal exit; so
   * do proper cleanup here for both cases. */
  g_free (profile_dir);
  g_free (key);
  /* if we had an error then we're going to return NULL as the id */
  if (err != NULL)
  {
    g_free (profile_id);
    profile_id = NULL;
  }

  g_list_free (profiles);

  if (id_list)
  {
    g_slist_foreach (id_list, (GFunc) g_free, NULL);
    g_slist_free (id_list);
  }

  /* FIXME
  if (err)
    {
      if (GTK_WIDGET_VISIBLE (transient_parent))
        {
          GtkWidget *dialog;

          dialog = gtk_message_dialog_new (GTK_WINDOW (transient_parent),
                                           GTK_DIALOG_DESTROY_WITH_PARENT,
                                           GTK_MESSAGE_ERROR,
                                           GTK_BUTTONS_CLOSE,
                                           _("There was an error creating the profile \"%s\""),
                                           visible_id);
          g_signal_connect (G_OBJECT (dialog), "response",
                            G_CALLBACK (gtk_widget_destroy),
                            NULL);

          dialog_add_details (GTK_DIALOG (dialog),
                              err->message);

          gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);

          gtk_widget_show (dialog);
        }

      g_error_free (err);
    }
  */
  if (err)
  {
    GMP_DEBUG("WARNING: error: %s !\n", err->message);
    *error = err;
  }

  //g_object_unref (G_OBJECT (transient_parent));
  GMP_DEBUG("a_p_c: done\n");
  return profile_id;
}

/* delete the given list of profiles from the gconf profile_list key */
void
gm_audio_profile_delete_list (GConfClient *conf,
                           GList       *deleted_profiles,
                           GError      **error)
{
  GList *current_profiles;
  GList *tmp;
  GSList *id_list;
  GError *err;

  current_profiles = gm_audio_profile_get_list ();

  /* remove deleted profiles from list */
  tmp = deleted_profiles;
  while (tmp != NULL)
  {
    GMAudioProfile *profile = tmp->data;

    current_profiles = g_list_remove (current_profiles, profile);

    tmp = tmp->next;
  }

  /* make list of profile ids */
  id_list = NULL;
  tmp = current_profiles;
  while (tmp != NULL)
  {
    id_list = g_slist_prepend (id_list,
                               g_strdup (gm_audio_profile_get_id (tmp->data)));

    tmp = tmp->next;
  }

  g_list_free (current_profiles);
  err = NULL;
  GMP_DEBUG("setting profile_list in GConf\n");
  gconf_client_set_list (conf,
                         CONF_GLOBAL_PREFIX"/profile_list",
                         GCONF_VALUE_STRING,
                         id_list,
                         &err);

  g_slist_foreach (id_list, (GFunc) g_free, NULL);
  g_slist_free (id_list);

  if (err && error) *error = err;
}
