#pragma once

/**
 * A "plugin init function" type.
 */
typedef int (*plugin_init_fn)(void);

/**
 * Union used to convert between void* and plugin_init_fn
 * without violating ISO C standards.
 */
union fn_ptr {
    void* obj;
    plugin_init_fn fn;
};

void load_plugins(void);

int initialize_plugin(const char* path);
