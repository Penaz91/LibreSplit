#pragma once

#include "gui/component/components.h"
#include "plugins/plugin_utils.h"

typedef int (*register_component_func)(char* name, ls_component_new_func fn);
int register_plugin_component(char* name, ls_component_new_func fn);

typedef struct PlugAPIGTK {
    PlugAPI* core_api;
    register_component_func register_component;
} PlugAPIGTK;
