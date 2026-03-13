#pragma once

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

int register_lua_function(const char* name, void* fn);

int register_event_hook(HookableEvent event, void* fn);
