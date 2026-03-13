#pragma once
#include "../lasr/auto-splitter.h"
#include "lua.h"

extern int external_luac_function_count;
extern int external_luac_function_size;
extern lasr_function* external_luac_functions;

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

typedef struct PlugAPI {
    register_lua_func register_lua_function;
} PlugAPI;

int register_lua_function(const char* name, lua_CFunction);

int register_event_hook(HookableEvent event, void* fn);

void init_external_lasr_functions(void);
