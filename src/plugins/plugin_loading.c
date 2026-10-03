#include "plugins/plugin_loading.h"
#include "logging.h"
#include "plugins/plugin_interfaces.h"
#include "plugins/plugin_utils.h"
#include "settings/utils.h"
#include <assert.h>
#include <dirent.h>
#include <dlfcn.h>
#include <gio/gio.h>
#include <gio/gmenu.h>
#include <gio/gmenumodel.h>
#include <glib-object.h>
#include <glib.h>
#include <libpeas.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

PlugAPI api = {
    .register_lua_function = register_lua_function,
    .register_event_hook = register_event_hook,
    .register_component = register_plugin_component,
};

static PeasEngine* plugin_engine = NULL;
GPtrArray* active_plugins = NULL;

/**
 * Frees the plugin and its contents
 *
 * @param plugin The Plugin to remove from memory
 */
static void plugin_free(Plugin* plugin)
{
    if (plugin == NULL) {
        return;
    }
    g_clear_object(&plugin->base);
    g_clear_object(&plugin->contextmenu);
    g_clear_object(&plugin->lasr);
    g_clear_object(&plugin->components);
    g_clear_object(&plugin->event_listener);
    g_clear_object(&plugin->info);

    g_free(plugin);
}

/**
 * Utility function to load "optional extensions" that go beyond the basic
 * plugin lifecycle
 *
 * @param inf The Plugin Info struct
 * @param extension_type The Type of extension to try and load
 * @param name The name of the extension
 *
 * @return A pointer to the loaded GObject or NULL
 */
static GObject* create_optional_extension(PeasPluginInfo* info, GType extension_type, const char* extension_name)
{
    GObject* extension = peas_engine_create_extension(plugin_engine, info, extension_type, NULL);
    if (extension == NULL) {
        LOG_DEBUGF("Plugin %s does not implement extension %s", peas_plugin_info_get_name(info), extension_name);
    }
    return extension;
}

static int load_plugin_info(PeasPluginInfo* info)
{
    GError* err = NULL;
    Plugin* plugin = NULL;

    if (!peas_engine_load_plugin(plugin_engine, info)) {
        LOG_WARNF("Unable to load plugin: %s", peas_plugin_info_get_name(info));
        return -1;
    }
    plugin = g_new0(Plugin, 1);

    plugin->info = g_object_ref(info);

    // PluginBase is required
    plugin->base = peas_engine_create_extension(plugin_engine, info, PLUGIN_TYPE_BASE, NULL);

    if (plugin->base == NULL) {
        LOG_WARNF("Plugin %s does not implement the required base interface", peas_plugin_info_get_name(info));
        goto fail;
    }

    // Optional Interfaces
    plugin->contextmenu = create_optional_extension(info, PLUGIN_TYPE_CONTEXTMENU, "PluginContextMenu");
    plugin->lasr = create_optional_extension(info, PLUGIN_TYPE_LASR, "PluginLASR");
    plugin->components = create_optional_extension(info, PLUGIN_TYPE_COMPONENTS, "PluginComponents");
    plugin->event_listener = create_optional_extension(info, PLUGIN_TYPE_EVENTLISTENER, "PluginEventListener");

    // Initialize the base extension

    PluginBase* base = PLUGIN_BASE(plugin->base);
    PluginBaseInterface* iface = PLUGIN_BASE_GET_IFACE(base);

    if (iface->init && !iface->init(base, &api, &err)) {
        LOG_WARNF("Plugin initialization failed for plugin %s: %s", peas_plugin_info_get_name(info), err ? err->message : "Unknown Error");
        goto fail;
    }

    // TODO: [Penaz] [2026-10-03] Add eventual optional extensions loading here
    //
    // XXX: [Penaz] [2026-10-03] How would I register the context menu extension here, since
    // ^ I don't have a pointer to the context menu itself?

    g_ptr_array_add(active_plugins, plugin);
    LOG_INFOF("Loaded Plugin %s", peas_plugin_info_get_name(info));
    return 0;

fail:
    g_clear_error(&err);
    plugin_free(plugin);
    peas_engine_unload_plugin(plugin_engine, info);
    return -1;
}

/**
 * Shows the plugins that have been loaded by LibPeas2
 */
static void log_available_plugins(void)
{
    guint item_no;

    item_no = g_list_model_get_n_items(G_LIST_MODEL(plugin_engine));

    for (guint i = 0; i < item_no; i++) {
        PeasPluginInfo* info;
        const char* name;
        const char* descr;
        const char* version;
        /*const char* const* authors;*/

        info = g_list_model_get_item(G_LIST_MODEL(plugin_engine), i);

        name = peas_plugin_info_get_name(info);
        descr = peas_plugin_info_get_description(info);
        version = peas_plugin_info_get_version(info);
        /*authors = peas_plugin_info_get_authors(info);*/

        LOG_INFOF("Found Plugin: %s (v%s) - %s", name ? name : "(unnamed)", version ? version : "(unknown)", descr ? descr : "(no description)");

        g_object_unref(info);
    }
}

/**
 * Prepares the LibPeas2 plugin registry to be used.
 *
 * @returns Zero if everything went well. An error code otherwise.
 */
int initialize_plugin_registry(void)
{
    if (plugin_engine != NULL) {
        LOG_WARN("Trying to load an already-initialized plugin engine");
        return 0;
    }
    plugin_engine = peas_engine_new();

    active_plugins = g_ptr_array_new_with_free_func(g_object_unref);

    char plugdir[PATH_MAX];
    get_libresplit_data_folder_path(plugdir);
    strlcat(plugdir, "/plugins/", sizeof(plugdir));

    peas_engine_add_search_path(plugin_engine, plugdir, plugdir);

    peas_engine_rescan_plugins(plugin_engine);

    LOG_INFO("Plugins loaded via Libpeas2");

    log_available_plugins();
    return 0;
}

/**
 * Calls, in turn, all the register_context_menu functions
 * of the plugins, to fill in the Plugins submenu.
 *
 * @param parent The Plugins submenu.
 * @returns 0 if everything went well. An error code otherwise.
 */
int create_plugin_context_menus(GMenu* parent, GtkWidget* window)
{
    // XXX: [Penaz] [2026-08-23] As things are now, each plugin has full control
    // ^ over the "Plugins" submenu. Ideally we would want to isolate each plugin
    // ^ into its own submenu to reduce interactions.

    guint item_no = 0;
    GMenu* submenu = g_menu_new();

    for (guint i = 0; i < active_plugins->len; i++) {
        Plugin* plugin;
        PluginContextmenu* extension;
        PluginContextmenuInterface* iface;
        GError* err = NULL;
        guint before;
        guint after;

        plugin = g_ptr_array_index(active_plugins, i);

        if (plugin->contextmenu == NULL) {
            // This plugin doesn't have the "write to context menu" extension
            continue;
        }

        extension = PLUGIN_CONTEXTMENU(plugin->contextmenu);
        iface = PLUGIN_CONTEXTMENU_GET_IFACE(extension);

        if (iface->register_menu == NULL) {
            // This plugin has the extension but not the function?
            continue;
        }

        before = g_menu_model_get_n_items(G_MENU_MODEL(submenu));

        if (!iface->register_menu(extension, submenu, window, &err)) {
            LOG_WARNF("Context menu registration failed for plugin %s: %s", peas_plugin_info_get_name(plugin->info), err ? err->message : "Unknown Error");
            g_clear_error(&err);
            continue;
        }

        after = g_menu_model_get_n_items(G_MENU_MODEL(submenu));

        item_no += after - before;
    }

    if (item_no == 0) {
        // If, by the end of the registration, there are no items in the "Plugins"
        // submenu, just fill it with a placeholder entry.
        g_menu_append(submenu, "No plugin entries.", NULL);
    }
    // Set the newly created submenu to the "Plugins" entry in the context menu
    g_menu_append_submenu(parent, "Plugins", G_MENU_MODEL(submenu));
    g_object_unref(submenu);

    return 0;
}

/**
 * General function that loads plugins
 */
void load_plugins(void)
{
    if (initialize_plugin_registry() != 0) {
        LOG_ERR("Cannot initialize plugin engine");
        return;
    }

    guint item_no = g_list_model_get_n_items(G_LIST_MODEL(plugin_engine));

    for (guint i = 0; i < item_no; i++) {
        GError* err;
        PeasPluginInfo* info;
        info = g_list_model_get_item(G_LIST_MODEL(plugin_engine), i);

        if (!peas_plugin_info_is_available(info, &err)) {
            LOG_DEBUGF("Plugin %s not available: %s", peas_plugin_info_get_name(info), err ? err->message : "Unknown error");
            g_clear_error(&err);
            g_object_unref(info);
            continue;
        }

        if (!peas_plugin_info_is_hidden(info)) {
            load_plugin_info(info);
        }

        g_object_unref(info);
    }
}

int unload_plugins(void)
{
    if (active_plugins == NULL) {
        return 0;
    }

    // NOTE: [Penaz] [2026-10-03] If the plugin extensions have their own custom shutdown callback
    // ^ it should be called here, before we exterminate everything. Should we give a shutdown callback
    // ^ at the base level?

    g_ptr_array_set_size(active_plugins, 0);

    guint item_no = g_list_model_get_n_items(G_LIST_MODEL(plugin_engine));

    for (guint i = 0; i < item_no; i++) {
        PeasPluginInfo* info;

        info = g_list_model_get_item(G_LIST_MODEL(plugin_engine), i);

        if (peas_plugin_info_is_loaded(info)) {
            peas_engine_unload_plugin(plugin_engine, info);
        }
        g_object_unref(info);
    }

    g_clear_pointer(&active_plugins, g_ptr_array_unref);

    return 0;
}
