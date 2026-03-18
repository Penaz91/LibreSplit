#pragma once
#include "../plugins/plugin_utils.h"

// Plugin metadata
extern const char plugin_name[];
extern const char plugin_description[];
extern const char plugin_version[];
extern const char plugin_author[];

// Functions to connect host and plugin
int plug_init(PlugAPI* api);
int plug_shutdown(void);
