#include "include/plugins/plugin_loading.h"
#include "include/logging.h"
#include "include/plugins/plugin_utils.h"
#include "include/settings/utils.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

PlugAPI api = {
    .register_lua_function = register_lua_function,
    .register_event_hook = register_event_hook
};

static PluginRegistry plugin_registry = {
    .count = 0,
    .size = 2,
    .plugins = NULL,
};

/**
 * Extracts the plugin metadata from the shared object without executing its code.
 *
 * @param[in] path The path to the plugin
 * @param[out] name The name of the plugin
 * @param[out] description The description of the plugin
 * @param[out] version The version of the plugin
 * @param[out] author The author of the plugin
 *
 * @returns 0 if everything went well. An error code otherwise.
 */
static int get_plugin_metadata(const char* path, char** name, char** description, char** version, char** author)
{
    // Loads the plugin lazily, not executing any code
    void* plugin_handle = dlopen(path, RTLD_LAZY);
    if (!plugin_handle) {
        LOG_WARNF("Unable to open plugin for inspection: %s", path);
        LOG_WARNF("%s", dlerror());
        return -1;
    }

    const char* plugin_name = (const char*)dlsym(plugin_handle, "plugin_name");
    const char* plugin_desc = (const char*)dlsym(plugin_handle, "plugin_description");
    const char* plugin_version = (const char*)dlsym(plugin_handle, "plugin_version");
    const char* plugin_author = (const char*)dlsym(plugin_handle, "plugin_author");

    if (!plugin_name || !plugin_desc || !plugin_version || !plugin_author) {
        LOG_WARNF("Plugin missing some metadata: %s", path);
        dlclose(plugin_handle);
        return -1;
    }

    *name = strdup(plugin_name);
    *description = strdup(plugin_desc);
    *version = strdup(plugin_version);
    *author = strdup(plugin_author);

    dlclose(plugin_handle);
    return 0;
}

/**
 * General function that loads plugins
 */
void load_plugins(void)
{
    initialize_plugin_registry();
    // HACK: [Penaz] [2026-03-15] Reduce the datadir size to account for the filename concat
    // ^ and the "plugins/" addition, and some slack
    char plugdir[PATH_MAX - 300];
    get_libresplit_data_folder_path(plugdir);
    strcat(plugdir, "/plugins");
    DIR* dir = opendir(plugdir);
    if (!dir) {
        LOG_WARN("Unable to open plugins directory");
        return;
    }

    struct dirent* ent;

    while ((ent = readdir(dir))) {
        const char* extension = strrchr(ent->d_name, '.');
        if (!extension || strcmp(extension, ".so") != 0) {
            LOG_DEBUGF("Ignoring non-plugin %s", ent->d_name);
            continue;
        }

        char path[PATH_MAX];
        // If we didn't reduce the plugdir buffer size, GCC would complain about
        // possible buffer overflows here.
        snprintf(path, sizeof(path), "%s/%s", plugdir, ent->d_name);

        // Initialize the metadata
        char *name = NULL, *desc = NULL, *version = NULL, *author = NULL;
        if (get_plugin_metadata(path, &name, &desc, &version, &author) == 0) {
            LOG_INFOF("Found plugin: %s (v%s) by %s - %s", name, version, author, desc);
            free(name);
            free(desc);
            free(version);
            free(author);
            // TODO: [Penaz] [2026-03-12] Make this optional according to user settings?
            // FIXME: [Penaz] [2026-03-14] Remember to dlclose at the end
            initialize_plugin(path);
        }
    }
    closedir(dir);
}

/**
 * Takes a path to a .so file and calls its "plug_init" function.
 *
 * @param[in] path The path to the plugin.
 *
 * @returns 0 if everything goes well or an error code otherwise.
 */
int initialize_plugin(const char* path)
{
    if (plugin_registry.count == plugin_registry.size) {
        plugin_registry.size *= 2;
        plugin_registry.plugins = realloc(plugin_registry.plugins, plugin_registry.size * sizeof(Plugin));
    }
    void* handle = dlopen(path, RTLD_NOW);
    if (!handle) {
        LOG_WARNF("Unable to open plugin %s: %s", path, dlerror());
        return -1;
    }

    void* sym = dlsym(handle, "plug_init");

    union init_fn_ptr u;
    u.obj = sym;
    plugin_init_fn init_function = u.fn;

    if (!init_function) {
        LOG_WARNF("Cannot load plugin init function: %s", path);
        dlclose(handle);
        return -1;
    }

    if (init_function(&api) != 0) {
        LOG_WARNF("Plugin init function failed: %s", path);
        dlclose(handle);
        return -1;
    }

    plugin_registry.plugins[plugin_registry.count].handle = handle;
    plugin_registry.plugins[plugin_registry.count].path = strdup(path);
    plugin_registry.count++;

    return 0;
}

/**
 * Prepares the plugin registry to be used.
 *
 * @returns Zero if everything went well. An error code otherwise.
 */
int initialize_plugin_registry(void)
{
    LOG_INFO("Initializing plugin registry");
    plugin_registry.plugins = malloc(plugin_registry.size * sizeof(Plugin));
    if (!plugin_registry.plugins) {
        LOG_WARN("Plugin registry initialization failed.");
        abort();
    }
    return 0;
}

/**
 * Closes the handlers for all plugins and frees memory-
 *
 * NOTE: Currently unused.
 *
 * @returns Zero if everything went well, an error code otherwise
 */
int unload_plugins(void)
{
    LOG_INFO("Closing plugin handlers");
    for (int i = 0; i < plugin_registry.count; i++) {
        LOG_DEBUGF("Calling shutdown function for plugin %s", plugin_registry.plugins[i].path);
        void* sym = dlsym(plugin_registry.plugins[i].handle, "plug_shutdown");
        union shutdown_fn_ptr u;
        u.obj = sym;
        plugin_shutdown_fn shutdown_function = u.fn;
        if (!shutdown_function) {
            LOG_WARNF("No shutdown function defined for plugin %s", plugin_registry.plugins[i].path);
        } else {
            if (shutdown_function() != 0) {
                LOG_WARNF("Shutdown function for plugin %s failed", plugin_registry.plugins[i].path);
            }
        }
        LOG_DEBUGF("Closing handlers for plugin %s", plugin_registry.plugins[i].path);
        // Close the dynamic linking handler
        dlclose(plugin_registry.plugins[i].handle);
        plugin_registry.plugins[i].handle = NULL;
        free(plugin_registry.plugins[i].path);
    }
    free(plugin_registry.plugins);
    return 0;
}
