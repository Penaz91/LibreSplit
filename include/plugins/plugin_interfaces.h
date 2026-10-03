#pragma once

#include "plugins/plugin_utils.h"
#include <gio/gio.h>
#include <glib-object.h>
#include <glibconfig.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

// The base core functionality of the plugin
#define PLUGIN_TYPE_BASE (plugin_base_get_type())
G_DECLARE_INTERFACE(PluginBase, plugin_base, PLUGIN, BASE, GObject)

struct _PluginBaseInterface {
    GTypeInterface parent_iface;

    int (*init)(PluginBase* self, const PlugAPI* api, GError** err);
};

// NOTE: [Penaz] [2026-10-03] Interfaces for future separation

// To implement if the plugin needs to add stuff to the context menu
#define PLUGIN_TYPE_CONTEXTMENU (plugin_contextmenu_get_type())
G_DECLARE_INTERFACE(PluginContextmenu, plugin_contextmenu, PLUGIN, CONTEXTMENU, GObject)

struct _PluginContextmenuInterface {
    GTypeInterface parent_iface;

    int (*register_menu)(PluginContextmenu* self, GMenu* menu, GtkWidget* window, GError** err);
};

// To define if the plugin needs to add Lua functions
#define PLUGIN_TYPE_LASR (plugin_lasr_get_type())
G_DECLARE_INTERFACE(PluginLASR, plugin_lasr, PLUGIN, LASR, GObject)

struct _PluginLASRInterface {
    GTypeInterface parent_iface;

    int (*register_lua_functions)(PluginLASR* self, const PlugAPI* api, GError** err);
};

// To define if the plugin needs to add GUI components
#define PLUGIN_TYPE_COMPONENTS (plugin_components_get_type())
G_DECLARE_INTERFACE(PluginComponents, plugin_components, PLUGIN, COMPONENTS, GObject)

struct _PluginComponentsInterface {
    GTypeInterface parent_iface;
    int (*register_plugin_component)(PluginComponents* self, char* name, const PlugAPI* api, GError** err);
};

// To define if the plugin hooks to an event
#define PLUGIN_TYPE_EVENTLISTENER (plugin_event_listener_get_type())
G_DECLARE_INTERFACE(PluginEventListener, plugin_event_listener, PLUGIN, EVENT_LISTENER, GObject)

struct _PluginEventListenerInterface {
    GTypeInterface parent_iface;

    int (*register_events)(PluginEventListener* self, const PlugAPI* api, GError** err);
};

G_END_DECLS
