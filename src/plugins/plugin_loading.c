#include "plugin_loading.h"
#include "../logging.h"
#include <dirent.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int get_plugin_metadata(const char* path, char** name, char** description, char** version, char** author)
{
    // Loads the plugin lazily, not executing any code
    void* plugin_handle = dlopen(path, RTLD_LAZY);
    if (!plugin_handle) {
        LOG_WARNF("Unable to open plugin for inspection: %s", path);
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

void load_plugins(void)
{
    // TODO: [Penaz] [2026-03-11] Change to use XDG_DIRS
    DIR* dir = opendir("./plugins");
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
        // TODO: [Penaz] [2026-03-11] Change to use XDG_DIRS
        snprintf(path, sizeof(path), "./plugins/%s", ent->d_name);

        // Initialize the metadata
        char *name = NULL, *desc = NULL, *version = NULL, *author = NULL;
        if (get_plugin_metadata(path, &name, &desc, &version, &author) == 0) {
            LOG_INFOF("Found plugin: %s (v%s) by %s - %s", name, version, author, desc);
            // TODO: [Penaz] [2026-03-11] Really load the plugins instead of showing their metadata
            free(name);
            free(desc);
            free(version);
            free(author);
        }
    }
    closedir(dir);
}
