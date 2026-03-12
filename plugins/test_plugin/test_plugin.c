#include "plugin.h"
#include <stdio.h>

const char plugin_name[] = "Test Plugin";
const char plugin_description[] = "Does nothing, it just exists";
const char plugin_version[] = "0.1";
const char plugin_author[] = "The LibreSplit Core Team";

int plug_init(void)
{
    printf("Hello from a plugin!\n");
    return 0;
}
