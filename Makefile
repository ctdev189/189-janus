plugin: dev189_janus_plugin_foo.c
		clang `pkg-config --libs --cflags glib-2.0` \
		-dylib -Wl,-undefined,dynamic_lookup \
		-I/usr/local/janus/include/janus \
		-o libdev189_janus_plugin_foo.0.dylib dev189_janus_plugin_foo.c