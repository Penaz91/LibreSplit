#include "lua.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct WatchNode {
    char* name;
    struct WatchNode* next;
} WatchNode;

static WatchNode* watch_list = NULL;

static void add_watch(const char* name)
{
    WatchNode* node = malloc(sizeof(WatchNode));
    node->name = strdup(name);
    node->next = watch_list;
    watch_list = node;
}

static void print_lua_value(lua_State* L, const char* name)
{
    lua_getglobal(L, name); // push value to the top of the stack
    if (lua_isnil(L, -1)) {
        printf("%s = nil\n", name);
    } else if (lua_isnumber(L, -1)) {
        printf("%s = %g\n", name, lua_tonumber(L, -1));
    } else if (lua_isboolean(L, -1)) {
        printf("%s = %s\n", name, lua_toboolean(L, -1) ? "true" : "false");
    } else if (lua_isstring(L, -1)) {
        printf("%s = \"%s\"\n", name, lua_tostring(L, -1));
    } else {
        printf("%s = <unknown>\n", name);
    }
    lua_pop(L, 1); // clean stack
}

// Adds a global variable name to the list of names to be watched
int lua_watch(lua_State* L)
{
    if (lua_isstring(L, 1)) {
        const char* name = lua_tostring(L, 1);
        add_watch(name);
    }
    return 0;
}

// Prints all the balues in the watcher linked list to the console
int lua_updatewatcher(lua_State* L)
{
    for (WatchNode* n = watch_list; n != NULL; n = n->next) {
        print_lua_value(L, n->name);
    }
    return 0;
}
