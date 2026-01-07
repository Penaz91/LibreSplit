#ifndef __AUTO_SPLITTER_H__
#define __AUTO_SPLITTER_H__

#include "lua.h"
#include <linux/limits.h>
#include <stdatomic.h>

extern atomic_bool auto_splitter_enabled; /*!< Defines if the auto splitter is enabled */
extern atomic_bool auto_splitter_running; /*!< Defines if the auto splitter is running */
extern atomic_bool call_start; /*!< True if the auto splitter is requesting for a run to start */
extern atomic_bool run_started; /*!< Defines if a run is started */
extern atomic_bool run_finished; /*!< Defines if a run is finished */
extern atomic_bool call_split; /*!< True if the auto splitter is requesting to split */
extern atomic_bool call_split;
extern atomic_bool toggle_loading;
extern atomic_bool call_reset; /*!< True if the auto splitter is requesting a run reset */
extern atomic_bool update_game_time; /*!< True if the auto splitter is requesting the game time to be updated */
extern atomic_llong game_time_value; /*!< The in-game time value, in milliseconds */
extern char auto_splitter_file[PATH_MAX]; /*!< The path to the auto splitter Lua file */
extern int maps_cache_cycles_value; /*!< The number of cycles the cache is active for */

/**
 * Defines a Lua Auto Splitter Runtime Function.
 */
struct lasr_func_struct {
    char* function_name; /*!< The name of the function in Lua */
    lua_CFunction function_ptr; /*!< C Function to be executed */
} typedef lasr_function;

void check_directories();
void run_auto_splitter();

#endif /* __AUTO_SPLITTER_H__ */
