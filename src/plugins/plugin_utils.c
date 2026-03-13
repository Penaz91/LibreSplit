#include "plugin_utils.h"
#include "lua.h"
#include "src/logging.h"
#include <stdlib.h>
#include <string.h>

int external_luac_function_count = 0;
int external_luac_function_size = 2;
lasr_function* external_luac_functions = NULL;

/**
 * Register a new C function to be added to the Lua Auto Splitter Runtime.
 *
 * @param[in] name The name the function should get inside the Lua Environment.
 * @param[in] fn The C function to be registered.
 *
 * @returns 0 if everything went okay, otherwise an error code.
 */
int register_lua_function(const char* name, lua_CFunction fn)
{
    LOG_DEBUGF("Pushing %s to the external Lua Function array", name);
    if (external_luac_function_count == external_luac_function_size) {
        LOG_DEBUG("Reallocating array for size");
        // Resize array if too small
        external_luac_function_size *= 2;
        external_luac_functions = realloc(external_luac_functions, external_luac_function_size * sizeof(struct lasr_function));
        if (!external_luac_functions) {
            LOG_ERR("Cannot reallocate external Lua C function array");
            free(external_luac_functions);
            abort();
        }
    }
    // Add the new function to the array
    external_luac_functions[external_luac_function_count].function_name = strdup(name);
    if (!external_luac_functions[external_luac_function_count].function_name) {
        LOG_ERRF("Cannot allocate memory for the function named %s", name);
        abort();
    }
    external_luac_functions[external_luac_function_count].function_ptr = fn;
    external_luac_function_count++;
    // Null-terminate the array
    LOG_DEBUG("Null-terminating the external lua C function array");
    external_luac_functions[external_luac_function_count].function_name = NULL;
    external_luac_functions[external_luac_function_count].function_ptr = NULL;
    return 0;
}

/**
 * Register a plugin function to be called when a certain event happens.o
 *
 * @param[in] event The event to react to.
 * @param[in] fn The function pointer to be called.
 *
 * @returns 0 if everything went okay, otherwise an error code.
 */
int register_event_hook(HookableEvent event, void* fn)
{
    // TODO: [Penaz] [2026-03-13] STUB
    switch (event) {
        case START:
            LOG_ERR("Hooking to start event not implemented");
            break;
        case SPLIT:
            LOG_ERR("Hooking to split event not implemented");
            break;
        case STOP:
            LOG_ERR("Hooking to stop event not implemented");
            break;
        case RESET:
            LOG_ERR("Hooking to reset event not implemented");
            break;
        case CANCEL:
            LOG_ERR("Hooking to cancel event not implemented");
            break;
        case SKIP:
            LOG_ERR("Hooking to skip event not implemented");
            break;
        case UNSPLIT:
            LOG_ERR("Hooking to unsplit event not implemented");
            break;
        case PAUSE:
            LOG_ERR("Hooking to pause event not implemented");
            break;
        case UNPAUSE:
            LOG_ERR("Hooking to unpause event not implemented");
            break;
    }
    return -1;
}

void init_external_lasr_functions(void)
{
    LOG_DEBUG("Initializing external LuaC functions array");
    // First array allocation
    // FIXME: [Penaz] [2026-03-13] Remember to free the external luac_functions array!
    external_luac_functions = malloc(external_luac_function_size * sizeof(struct lasr_function));
    external_luac_functions[0].function_name = NULL;
    external_luac_functions[0].function_ptr = NULL;
}
