#include "plugins/plugin_interfaces.h"
#include <glib-object.h>

G_DEFINE_INTERFACE(PluginBase, plugin_base, G_TYPE_OBJECT)

static void plugin_base_default_init(PluginBaseInterface* iface)
{
    (void)iface;
}

// NOTE: [Penaz] [2026-10-03] Implementations for future separation

G_DEFINE_INTERFACE(PluginContextmenu, plugin_contextmenu, G_TYPE_OBJECT)

static void plugin_contextmenu_default_init(PluginContextmenuInterface* iface)
{
    (void)iface;
}

G_DEFINE_INTERFACE(PluginLASR, plugin_lasr, G_TYPE_OBJECT)

static void plugin_lasr_default_init(PluginLASRInterface* iface)
{
    (void)iface;
}

G_DEFINE_INTERFACE(PluginComponents, plugin_components, G_TYPE_OBJECT)

static void plugin_components_default_init(PluginComponentsInterface* iface)
{
    (void)iface;
}

G_DEFINE_INTERFACE(PluginEventListener, plugin_event_listener, G_TYPE_OBJECT)

static void plugin_event_listener_default_init(PluginEventListenerInterface* iface)
{
    (void)iface;
}
