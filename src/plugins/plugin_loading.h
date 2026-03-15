#pragma once
#include "plugin_utils.h"

/**
 * A plugin structure, keeps tabs on the handler
 * given by dlopen() and the path
 */
typedef struct _Plugin {
    char* path; /*!< Path of the loaded plugin */
    void* handle; /*!< Handle of the .so file, used to dlclose */
} Plugin;

/**
 * A plugin registry, used to keep in memory
 * handles to all plugins loaded.
 */
typedef struct _PluginRegistry {
    int count; /*!< Number of loaded and active plugins */
    int size; /*!< Size of the plugin registry */
    Plugin* plugins; /*!< Array of Plugin structs with all their handles. */
} PluginRegistry;

/**
 * A "plugin init function" type.
 */
typedef int (*plugin_init_fn)(PlugAPI*);

/**
 * Union used to convert between void* and plugin_init_fn
 * without violating ISO C standards.
 */
union fn_ptr {
    void* obj; /*!< Object pointer */
    plugin_init_fn fn; /*!< Function pointer */
};

void load_plugins(void);
int unload_plugins(void);

int initialize_plugin(const char* path);
int initialize_plugin_registry(void);
