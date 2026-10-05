#pragma once
#include "lua.h"
#include <stdint.h>

typedef uint32_t abi_version_t;
// External declarations
typedef struct ls_timer ls_timer;
typedef struct _ExternalLASRFunctionRegistry ExternalLASRFunctionRegistry;
typedef struct LSComponent LSComponent;
typedef struct LSComponentOps LSComponentOps;
typedef LSComponent* (*ls_component_new_func)(void);
typedef int (*timer_hook_func)(const ls_timer* timer);

/*! \enum event
 *
 *  Describes the events you can register an event hook for.
 */
typedef enum HookableEvent {
    START,
    SPLIT,
    STOP,
    RESET,
    CANCEL,
    SKIP,
    UNSPLIT,
    PAUSE,
    UNPAUSE,
} HookableEvent;
typedef int (*register_lua_func)(const char*, lua_CFunction);
typedef int (*register_event_func)(HookableEvent event, timer_hook_func fn);
typedef int (*register_component_func)(char* name, ls_component_new_func fn);

typedef struct PlugAPI {
    abi_version_t abi_version;
    register_lua_func register_lua_function;
    register_event_func register_event_hook;
    register_component_func register_component;
} PlugAPI;
