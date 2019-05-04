#include <plugins/plugin.h>
#include <debug.h>
#include <config.h>

#define DEV189_JANUS_PLUGIN_FOO_VERSION 1
#define DEV189_JANUS_PLUGIN_FOO_VERSION_STRING "0.0.1"
#define DEV189_JANUS_PLUGIN_FOO_DESCRIPTION "The simplest possible Janus plugin."
#define DEV189_JANUS_PLUGIN_FOO_NAME "JANUS hello world plugin"
#define DEV189_JANUS_PLUGIN_FOO_AUTHOR "Jason Young"
#define DEV189_JANUS_PLUGIN_FOO_PACKAGE "dev189.janus.plugin.foo"

janus_plugin *create(void);
int dev189_janus_plugin_foo_init(janus_callbacks *callback, const char *config_path);
void dev189_janus_plugin_foo_destroy(void);
int dev189_janus_plugin_foo_get_api_compatibility(void);
int dev189_janus_plugin_foo_get_version(void);
const char *dev189_janus_plugin_foo_get_version_string(void);
const char *dev189_janus_plugin_foo_get_description(void);
const char *dev189_janus_plugin_foo_get_name(void);
const char *dev189_janus_plugin_foo_get_author(void);
const char *dev189_janus_plugin_foo_get_package(void);
void dev189_janus_plugin_foo_create_session(janus_plugin_session *handle, int *error);
struct janus_plugin_result *dev189_janus_plugin_foo_handle_message(janus_plugin_session *handle, char *transaction, json_t *message, json_t *jsep);
void dev189_janus_plugin_foo_setup_media(janus_plugin_session *handle);
void dev189_janus_plugin_foo_incoming_rtp(janus_plugin_session *handle, int video, char *buf, int len);
void dev189_janus_plugin_foo_incoming_rtcp(janus_plugin_session *handle, int video, char *buf, int len);
void dev189_janus_plugin_foo_incoming_data(janus_plugin_session *handle, char *buf, int len);
void dev189_janus_plugin_foo_slow_link(janus_plugin_session *handle, int uplink, int video);
void dev189_janus_plugin_foo_hangup_media(janus_plugin_session *handle);
void dev189_janus_plugin_foo_destroy_session(janus_plugin_session *handle, int *error);
json_t *dev189_janus_plugin_foo_query_session(janus_plugin_session *handle);

static janus_plugin dev189_janus_plugin_foo =
    JANUS_PLUGIN_INIT(.init = dev189_janus_plugin_foo_init,
                      .destroy = dev189_janus_plugin_foo_destroy,
                      .get_api_compatibility = dev189_janus_plugin_foo_get_api_compatibility,
                      .get_version = dev189_janus_plugin_foo_get_version,
                      .get_version_string = dev189_janus_plugin_foo_get_version_string,
                      .get_description = dev189_janus_plugin_foo_get_description,
                      .get_name = dev189_janus_plugin_foo_get_name,
                      .get_author = dev189_janus_plugin_foo_get_author,
                      .get_package = dev189_janus_plugin_foo_get_package,
                      .create_session = dev189_janus_plugin_foo_create_session,
                      .handle_message = dev189_janus_plugin_foo_handle_message,
                      .setup_media = dev189_janus_plugin_foo_setup_media,
                      .incoming_rtp = dev189_janus_plugin_foo_incoming_rtp,
                      .incoming_rtcp = dev189_janus_plugin_foo_incoming_rtcp,
                      .incoming_data = dev189_janus_plugin_foo_incoming_data,
                      .slow_link = dev189_janus_plugin_foo_slow_link,
                      .hangup_media = dev189_janus_plugin_foo_hangup_media,
                      .destroy_session = dev189_janus_plugin_foo_destroy_session,
                      .query_session = dev189_janus_plugin_foo_query_session, );

janus_plugin *create(void)
{
    JANUS_LOG(LOG_VERB, "%s created!\n", DEV189_JANUS_PLUGIN_FOO_NAME);
    return &dev189_janus_plugin_foo;
}

int dev189_janus_plugin_foo_init(janus_callbacks *callback, const char *config_path)
{
    if (callback == NULL || config_path == NULL)
    {
        /* Invalid arguments */
        return -1;
    }

    /* Read configuration */
    char filename[255];
    g_snprintf(filename, 255, "%s/%s.jcfg", config_path, DEV189_JANUS_PLUGIN_FOO_PACKAGE);
    JANUS_LOG(LOG_VERB, "Configuration file: %s\n", filename);
    janus_config *config = janus_config_parse(filename);
    if (config == NULL)
    {
        JANUS_LOG(LOG_WARN, "Couldn't find .jcfg configuration file (%s), trying .cfg\n", DEV189_JANUS_PLUGIN_FOO_PACKAGE);
        g_snprintf(filename, 255, "%s/%s.cfg", config_path, DEV189_JANUS_PLUGIN_FOO_PACKAGE);
        JANUS_LOG(LOG_VERB, "Configuration file: %s\n", filename);
        config = janus_config_parse(filename);
    }
    if (config == NULL)
    {
        /* No config means no JS script */
        JANUS_LOG(LOG_ERR, "Failed to load configuration file for Duktape plugin...\n");
        return -1;
    }

    janus_config_print(config);

    JANUS_LOG(LOG_INFO, "%s initialized!\n", DEV189_JANUS_PLUGIN_FOO_NAME);
    return 0;
}

void dev189_janus_plugin_foo_destroy(void)
{
    JANUS_LOG(LOG_INFO, "%s destroyed!\n", DEV189_JANUS_PLUGIN_FOO_NAME);
}

int dev189_janus_plugin_foo_get_api_compatibility(void)
{
    return JANUS_PLUGIN_API_VERSION;
}

int dev189_janus_plugin_foo_get_version(void)
{
    return DEV189_JANUS_PLUGIN_FOO_VERSION;
}

const char *dev189_janus_plugin_foo_get_version_string(void)
{
    return DEV189_JANUS_PLUGIN_FOO_VERSION_STRING;
}

const char *dev189_janus_plugin_foo_get_description(void)
{
    return DEV189_JANUS_PLUGIN_FOO_DESCRIPTION;
}

const char *dev189_janus_plugin_foo_get_name(void)
{
    return DEV189_JANUS_PLUGIN_FOO_NAME;
}

const char *dev189_janus_plugin_foo_get_author(void)
{
    return DEV189_JANUS_PLUGIN_FOO_AUTHOR;
}

const char *dev189_janus_plugin_foo_get_package(void)
{
    return DEV189_JANUS_PLUGIN_FOO_PACKAGE;
}

void dev189_janus_plugin_foo_create_session(janus_plugin_session *handle, int *error)
{
    JANUS_LOG(LOG_INFO, "Session created.\n");
}

void dev189_janus_plugin_foo_destroy_session(janus_plugin_session *handle, int *error)
{
    //janus_refcount_decrease(&handle->ref);
    JANUS_LOG(LOG_INFO, "Session destroyed.\n");
}

json_t *dev189_janus_plugin_foo_query_session(janus_plugin_session *handle)
{
    //return json_object();
    return NULL;
}

struct janus_plugin_result *dev189_janus_plugin_foo_handle_message(janus_plugin_session *handle, char *transaction, json_t *message, json_t *jsep)
{
    //return janus_plugin_result_new(JANUS_PLUGIN_OK, NULL, json_object());
    return NULL;
}

void dev189_janus_plugin_foo_setup_media(janus_plugin_session *handle)
{
    JANUS_LOG(LOG_INFO, "WebRTC media is now available.\n");
}

void dev189_janus_plugin_foo_incoming_rtp(janus_plugin_session *handle, int video, char *buf, int len)
{
    JANUS_LOG(LOG_VERB, "Got an RTP message (%d bytes.)\n", len);
}

void dev189_janus_plugin_foo_incoming_rtcp(janus_plugin_session *handle, int video, char *buf, int len)
{
    JANUS_LOG(LOG_VERB, "Got an RTCP message (%d bytes.)\n", len);
}

void dev189_janus_plugin_foo_incoming_data(janus_plugin_session *handle, char *buf, int len)
{
    JANUS_LOG(LOG_VERB, "Got a DataChannel message (%d bytes.)\n", len);
}

void dev189_janus_plugin_foo_slow_link(janus_plugin_session *handle, int uplink, int video)
{
    JANUS_LOG(LOG_VERB, "Slow link detected.\n");
}

void dev189_janus_plugin_foo_hangup_media(janus_plugin_session *handle)
{
    JANUS_LOG(LOG_INFO, "No WebRTC media anymore.\n");
}