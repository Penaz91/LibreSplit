#ifndef __AUTO_SPLITTER_H__
#define __AUTO_SPLITTER_H__

#include "lua.h"
#include <linux/limits.h>
#include <stdatomic.h>

/**
 * Defines a Lua Auto Splitter Runtime Function.
 */
struct lasr_func_struct {
    char* function_name; /*!< The name of the function in Lua */
    lua_CFunction function_ptr; /*!< C Function to be executed */
} typedef lasr_function;

extern atomic_bool auto_splitter_enabled;
extern atomic_bool auto_splitter_running;
extern atomic_bool call_start;
extern atomic_bool run_started;
extern atomic_bool run_finished;
extern atomic_bool call_split;
extern atomic_bool toggle_loading;
extern atomic_bool call_reset;
extern atomic_bool update_game_time;
extern atomic_llong game_time_value;
extern char auto_splitter_file[PATH_MAX];
extern int maps_cache_cycles_value;

void check_directories();
void run_auto_splitter();

#endif /* __AUTO_SPLITTER_H__ */
