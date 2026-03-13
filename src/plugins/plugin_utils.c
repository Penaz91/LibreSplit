#include "plugin_utils.h"
#include "src/logging.h"

/**
 * Register a new C function to be added to the Lua Auto Splitter Runtime.
 *
 * @param[in] name The name the function should get inside the Lua Environment.
 * @param[in] fn The C function to be registered.
 *
 * @returns 0 if everything went okay, otherwise an error code.
 */
int register_lua_function(const char* name, void* fn)
{
    // TODO: [Penaz] [2026-03-13] STUB
    LOG_ERR("Functionality not implemented");
    return -1;
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
