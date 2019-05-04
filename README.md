# Janus Gateway

Janus Gateway是一个通用的WebRTC服务器，采用SFU（Selective Forwarding Unit）的方式进行媒体流的转发。Dev189Lab尝试以Janus为基础打造一个媒体服务器，用来处理通信能力开放平台中媒体流。

Janus通过插件的方式扩展功能。

>A list of plugins provided out of the box by Meetecho are documented in the Plugins documentation page: these plugins can be changed or extended to match your requirements, or just used as a simple reference should you be interested in writing a new plugin from scratch (and you're definitely welcome to!). A Plugin API to create new plugins, or understand how they're conceived, is documented as well.

Janus Gateway的代码中已经包含了几个插件，可以通过设置configure的参数控制安装哪些插件。实现新插件最简单的方法就是复制一个插件的代码，将代码修改成想要的名字，并修改configure.ac文件中对应的内容，重新执行./configure，make，make install就可以部署自己的插件。但是这种方式需要把自定义的插件代码和janus的代码混合在一起，不利于进行版本控制，所以需要找到一种独立开发插件的方法。

本文通过完成一个最小化的插件"dev189_janus_plugin_foo"，介绍独立开发插件的基本过程，同时该插件可以作为模板使用。

# Janus Plugin开发

Janus的插件机制是通过运行时加载动态链接库（dlopen）的方式实现，janus.c中main函数包含如下代码：

```
void *event = dlopen(eventpath, RTLD_NOW | RTLD_GLOBAL);
if (!event) {
    JANUS_LOG(LOG_ERR, "\tCouldn't load event handler plugin '%s': %s\n", eventent->d_name, dlerror());
} else {
    create_e *create = (create_e*) dlsym(event, "create");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
        JANUS_LOG(LOG_ERR, "\tCouldn't load symbol 'create': %s\n", dlsym_error);
        continue;
    }
    janus_eventhandler *janus_eventhandler = create();
    if(!janus_eventhandler) {
        JANUS_LOG(LOG_ERR, "\tCouldn't use function 'create'...\n");
        continue;
    }
    /* Are all the mandatory methods and callbacks implemented? */
    if(!janus_eventhandler->init || !janus_eventhandler->destroy ||
            !janus_eventhandler->get_api_compatibility ||
            !janus_eventhandler->get_version ||
            !janus_eventhandler->get_version_string ||
            !janus_eventhandler->get_description ||
            !janus_eventhandler->get_package ||
            !janus_eventhandler->get_name ||
            !janus_eventhandler->incoming_event) {
        JANUS_LOG(LOG_ERR, "\tMissing some mandatory methods/callbacks, skipping this event handler plugin...\n");
        continue;
    }
    if(janus_eventhandler->get_api_compatibility() < JANUS_EVENTHANDLER_API_VERSION) {
        JANUS_LOG(LOG_ERR, "The '%s' event handler plugin was compiled against an older version of the API (%d < %d), skipping it: update it to enable it again\n",
            janus_eventhandler->get_package(), janus_eventhandler->get_api_compatibility(), JANUS_EVENTHANDLER_API_VERSION);
        continue;
    }
    janus_eventhandler->init(configs_folder);
    JANUS_LOG(LOG_VERB, "\tVersion: %d (%s)\n", janus_eventhandler->get_version(), janus_eventhandler->get_version_string());
    JANUS_LOG(LOG_VERB, "\t   [%s] %s\n", janus_eventhandler->get_package(), janus_eventhandler->get_name());
    JANUS_LOG(LOG_VERB, "\t   %s\n", janus_eventhandler->get_description());
    JANUS_LOG(LOG_VERB, "\t   Plugin API version: %d\n", janus_eventhandler->get_api_compatibility());
    JANUS_LOG(LOG_VERB, "\t   Subscriptions:");
    if(janus_eventhandler->events_mask == 0) {
        JANUS_LOG(LOG_VERB, " none");
    } else {
        if(janus_flags_is_set(&janus_eventhandler->events_mask, JANUS_EVENT_TYPE_SESSION))
            JANUS_LOG(LOG_VERB, " sessions");
        if(janus_flags_is_set(&janus_eventhandler->events_mask, JANUS_EVENT_TYPE_HANDLE))
            JANUS_LOG(LOG_VERB, " handles");
        if(janus_flags_is_set(&janus_eventhandler->events_mask, JANUS_EVENT_TYPE_JSEP))
            JANUS_LOG(LOG_VERB, " jsep");
        if(janus_flags_is_set(&janus_eventhandler->events_mask, JANUS_EVENT_TYPE_WEBRTC))
            JANUS_LOG(LOG_VERB, " webrtc");
        if(janus_flags_is_set(&janus_eventhandler->events_mask, JANUS_EVENT_TYPE_MEDIA))
            JANUS_LOG(LOG_VERB, " media");
        if(janus_flags_is_set(&janus_eventhandler->events_mask, JANUS_EVENT_TYPE_PLUGIN))
            JANUS_LOG(LOG_VERB, " plugins");
        if(janus_flags_is_set(&janus_eventhandler->events_mask, JANUS_EVENT_TYPE_TRANSPORT))
            JANUS_LOG(LOG_VERB, " transports");
    }
    JANUS_LOG(LOG_VERB, "\n");
    if(eventhandlers == NULL)
        eventhandlers = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(eventhandlers, (gpointer)janus_eventhandler->get_package(), janus_eventhandler);
    if(eventhandlers_so == NULL)
        eventhandlers_so = g_hash_table_new(g_str_hash, g_str_equal);
    g_hash_table_insert(eventhandlers_so, (gpointer)janus_eventhandler->get_package(), event);
}
```

Janus打开动态链接库后，调用create函数，返回janus_plugin结构体。每个插件定义了结构体中要求的函数，实现插件特定的功能。

```
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
```

实现基本的插件框架并不复杂，但是在make时碰到问题，一些函数实现找不到。

```
Undefined symbols for architecture x86_64:
  "_janus_config_parse", referenced from:
      ...
  "_janus_config_print", referenced from:
      ...
  "_janus_log_colors", referenced from:
      ...
  "_janus_log_level", referenced from:
      ...
  "_janus_log_timestamps", referenced from:
      ...
  "_janus_vprintf", referenced from:
      ...
ld: symbol(s) not found for architecture x86_64
clang: error: linker command failed with exit code 1 (use -v to see invocation)
```

插件的代码中使用了JANUS_LOG，这个预定义在debug.h中，涉及到了log.c中实现的函数，例如：janus_vprintf函数。log并不是一个独立的链接库，而是在主程序中，所以make的时候会报错，找不到函数。Janus有很多基础的函数都在主程序中，所以必须找到一种方法让插件的动态链接库可以使用主程序中定义的函数。

动态链接库调用主程序中的方法，可以理解为主程序中定义了全局函数，各个插件可以调用。查看Janus的代码发现，confuigure.ac中包含-Wl,-export_dynamic，该选项的设置就是让动态链接库可以使用主程序中定义的内容，因此插件可以调用全局函数或或者全局变量。

>-export_dynamic: Preserves all global symbols in main executables during LTO.  Without this option, Link Time Optimization is allowed to inline and remove global functions. This option is used when a main executable may load a plug-in which requires certain symbols from the main executable.

但是还是存在链接时找不到函数符号的问题，不确定Janus是如何解决该问题的（似乎是用libtool的dlopening解决）。这里找到了一种方法，在Makefile中指定参数“-Wl,-undefined,dynamic_lookup”，这样就可以跳过找不到符号的问题。

>-undefined treatment: Specifies how undefined symbols are to be treated. Options are: error, warning, suppress, or dynamic_lookup. The default is error.

# 配置文件

Janus提供了通过配置文件指定插件运行时参数的机制，可以参照其自带插件配置文件的使用方式。创建一个配置文件，名称为插件的报名加“jcfg”后缀，dev189.janus.plugin.foo.jcfg，在插件的init方法中读取插件。

```
char filename[255];
g_snprintf(filename, 255, "%s/%s.jcfg", config_path, DEV189_JANUS_PLUGIN_FOO_PACKAGE);
JANUS_LOG(LOG_VERB, "Configuration file: %s\n", filename);
janus_config *config = janus_config_parse(filename);
```

# 部署
将make后的文件“libdev189_janus_plugin_foo.0.dylib”放到janus的插件目录“/usr/local/janus/lib/janus/plugins”。

配置文件放到janus的配置文件目录“/usr/local/janus/etc/janus”。

# 遗留问题

目前只实现了Mac环境的Makefile，应该扩展到各个环境。

链接时找不到符号的问题。

# 参考
[Janus官方文档](https://janus.conf.meetecho.com/docs/)

[Janus部署文档](https://github.com/meetecho/janus-gateway#dependencies)

[libtool: dlopening a module](https://www.gnu.org/software/libtool/manual/libtool.html#Dlopened-modules)

[插件开发例子1](https://github.com/mqp/janus-helloworld-plugin)

[插件开发例子2](https://github.com/michaelfranzl/janus-rtpforward-plugin)