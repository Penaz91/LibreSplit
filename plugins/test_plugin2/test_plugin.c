#include "lua.h"
#include "plugin.h"
#include "plugin_utils.h"
#include "timer.h"
#include <stdio.h>

const char plugin_name[] = "Test Plugin 2";
const char plugin_description[] = "Does something, it exists and yells at you for splitting";
const char plugin_version[] = "0.1";
const char plugin_author[] = "The LibreSplit Core Team";

int do_something_split(const ls_timer* timer)
{
    printf("Hey, I saw you split, you dirty splitter!");
    fflush(stdout);
    fflush(stderr);
    return 0;
}

int plug_init(PlugAPI* api)
{
    api->register_event_hook(SPLIT, do_something_split);
    return 0;
}
