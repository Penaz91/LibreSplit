#pragma once
#include "../lasr/auto-splitter.h"
#include "../timer.h"
#include "lua.h"

extern ExternalLASRFunctionRegistry external_lasr_functions;

extern TimerHookRegistry start_hooks;
extern TimerHookRegistry stop_hooks;
extern TimerHookRegistry split_hooks;
extern TimerHookRegistry reset_hooks;
extern TimerHookRegistry cancel_hooks;
extern TimerHookRegistry skip_hooks;
extern TimerHookRegistry unsplit_hooks;
extern TimerHookRegistry pause_hooks;
extern TimerHookRegistry unpause_hooks;

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

int register_event_hook(HookableEvent event, timer_hook_func fn);

void init_external_lasr_functions(void);
