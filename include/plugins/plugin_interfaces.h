#pragma once

#include "plugins/plugin_utils.h"
#include <glib-object.h>

// The base core functionality of the plugin
#define TYPE_PLUGIN_BASE (plugin_base_get_type())
G_DECLARE_INTERFACE(PluginBase, plugin_base, PLUGIN, BASE, GObject)

struct _PluginBaseInterface {
    GTypeInterface parent_interface;

    void (*init)(PluginBase* self, PlugAPI* api);
};

// To implement if the plugin needs to add stuff to the context menu
#define TYPE_PLUGIN_CONTEXTMENU (plugin_contextmenu_get_type())
G_DECLARE_INTERFACE(PluginContextmenu, plugin_contextmenu, PLUGIN, CONTEXTMENU, GObject)

struct _PluginContextmenuInterface {
    GTypeInterface parent_interface;

    int (*register_menu)(PluginContextmenu* self, GMenu* menu, GtkWidget* window);
};

// To define if the plugin needs to add Lua functions
#define TYPE_PLUGIN_LASR (plugin_lasr_get_type())
G_DECLARE_INTERFACE(PluginLASR, plugin_lasr, PLUGIN, LASR, GObject)

struct _PluginLASRInterface {
    GTypeInterface parent_interface;

    int (*register_lua_functions)(PluginLASR* self, PlugAPI* api);
};

// To define if the plugin needs to add GUI components
#define TYPE_PLUGIN_COMPONENTS (plugin_components_get_type())
G_DECLARE_INTERFACE(PluginComponents, plugin_components, PLUGIN, COMPONENTS, GObject)

struct _PluginComponentsInterface {
    GTypeInterface parent_interface;
    int (*register_plugin_component)(char* name, PlugAPI* api);
};

// To define if the plugin hooks to an event
#define TYPE_PLUGIN_EVENTLISTENER (plugin_event_listener_get_type())
G_DECLARE_INTERFACE(PluginEventListener, plugin_even_listener, PLUGIN, EVENT_LISTENER, GObject)

struct _PluginEventListenerInterface {
    GTypeInterface parent_interface;

    int (*register_events)(PluginEventListener* self, PlugAPI* api);
};
