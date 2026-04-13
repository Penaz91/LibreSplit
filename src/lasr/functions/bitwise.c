#include "bitwise.h"
#include <lua.h>
#include <stdio.h>

/**
 * Stand-in for bit.tohex, without the 8-bit limitation.
 *
 * Accepts at most 64bit-long numbers.
 *
 * @param L The lua stack
 */
int tohex(lua_State* L)
{
    if (lua_gettop(L) == 0 || lua_isnil(L, 1)) {
        printf("[tohex] One argument is required.");
        lua_pushnil(L);
        return 1;
    }

    if (lua_gettop(L) == 1) {
        if (!lua_isnumber(L, 1)) {
            printf("[tohex] The argument must be a number.");
            lua_pushnil(L);
            return 1;
        }

        unsigned long long number = lua_tointeger(L, 1);
        char to_return[66];
        snprintf(to_return, 66, "0x%llx", number);
        lua_pushstring(L, to_return);
        return 1;
    }
    return 1;
}
