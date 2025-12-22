#include "lua_utils.h"
#include "lua.h"
#include <stdio.h>

/**
 * The Lua "shallow copy table" function.
 *
 * A utility function to quickly shallow-copy tables in auto splitters
 * without having to copy values one-by one.
 *
 * @param L The Lua state
 */
int shallow_copy_tbl(lua_State* L)
{
    if (lua_gettop(L) == 0 || !lua_istable(L, 1)) {
        // Not a table, print a message and push nil to the stack
        printf("[shallow_copy_tbl] Argument is not a table or no argument passed.");
        lua_pushnil(L);
        return 1;
    }

    if (lua_gettop(L) > 1) {
        // Too many arguments
        printf("[shallow_copy_tbl] Too many arguments passed, only pass a single table");
        lua_pushnil(L);
        return 1;
    }

    // Argument is a table, begin the shallow copy process
    // Create a new table
    lua_newtable(L);
    // Stack: original_table, new_table
    int destination_index = lua_gettop(L);
    // Push a nil to the stack to bootstrap lua_next's key
    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        // Stack should be original_table, new_table, key, value
        // Old key is popped, Key is at -2, Value is at -1
        // Copy the key
        lua_pushvalue(L, -2);
        // Stack is now original_table, new_table, key, value, key
        // We need to swap the key and value to be able to use settable
        lua_insert(L, -2);
        // Now the stack is original_table, new_table, key, key, value
        lua_settable(L, destination_index);
        // After this, the new key and value should be popped from the stack
        // Stack: original_table, new_table, key, ready for next loop
    }
    // By the end, the stack should be original_table, new_table
    // so the new table should be already on top of the stack
    return 1;
}

/**
 * Utility function to convert a lua value to a string.
 *
 * Converts a value to a printable C string according to its type.
 * This is due to lua_tostring returning "null" for booleans and
 * other non-string types.
 */
const char* value_to_c_string(lua_State* L, int index)
{
    switch (lua_type(L, index)) {
        case LUA_TSTRING:
            return lua_tostring(L, index);
        case LUA_TNUMBER:
            return lua_tostring(L, index);
        case LUA_TBOOLEAN:
            return lua_toboolean(L, index) ? "true" : "false";
        case LUA_TNIL:
            return "nil";
        default:
            return "??";
    }
}

/**
 * The Lua "inspect and print table" command
 *
 * Takes a simple, non-nested table and prints it into console.
 * Useful for debugging purposes.
 *
 * Keys with "nil" as values won't be printed, cause on Lua
 * deleting a key-value pair from a table is done by assigning "nil" as value.
 *
 * @param L The Lua state
 */
int print_tbl(lua_State* L)
{
    if (lua_gettop(L) == 0 || !lua_istable(L, 1)) {
        // Not a table, print a message and push nil to the stack
        printf("[print_tbl] Argument is not a table or no argument passed.");
        return 0;
    }

    if (lua_gettop(L) > 1) {
        // Too many arguments
        printf("[shallow_copy_tbl] Too many arguments passed, only pass a single table");
        return 0;
    }

    // Argument is a table, begin the printing process
    // Push a nil to the stack to bootstrap lua_next's key
    lua_pushnil(L);
    // Stack: table, nil
    while (lua_next(L, 1) != 0) {
        // Old key is popped, Key is at -2, Value is at -1
        // Push the key to the stack to avoid confusing lua_next with
        // the call to lua_tostring (which can change the stack values)
        lua_pushvalue(L, -2);
        // Stack: table, key, value, key_copy
        const char* key_str = value_to_c_string(L, -1);
        const char* value_str = value_to_c_string(L, -2);
        printf("%s: %s\n", key_str, value_str);
        // Pop all except the original key
        lua_pop(L, 2);
    }
    return 0;
}

/**
 * Performs a binary "and" operation between two integers.
 *
 * @param L the Lua state.
 */
int b_and(lua_State* L)
{
    if (lua_gettop(L) != 2) {
        // Too few/many arguments passed
        printf("[b_and] Two arguments are required.");
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        // Arguments are not numbers
        printf("[b_and] Both arguments must be integers");
        return 0;
    }

    lua_Integer a = lua_tointeger(L, 1);
    lua_Integer b = lua_tointeger(L, 2);

    lua_Integer result = a & b;

    lua_pushinteger(L, result);
    return 1;
}

/**
 * Performs a binary "or" operation between two integers.
 *
 * @param L the Lua state.
 */
int b_or(lua_State* L)
{
    if (lua_gettop(L) != 2) {
        // Too few/many arguments passed
        printf("[b_or] Two arguments are required.");
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        // Arguments are not numbers
        printf("[b_or] Both arguments must be integers");
        return 0;
    }

    lua_Integer a = lua_tointeger(L, 1);
    lua_Integer b = lua_tointeger(L, 2);

    lua_Integer result = a | b;

    lua_pushinteger(L, result);
    return 1;
}

/**
 * Performs a binary "xor" operation between two integers.
 *
 * @param L the Lua state.
 */
int b_xor(lua_State* L)
{
    if (lua_gettop(L) != 2) {
        // Too few/many arguments passed
        printf("[b_xor] Two arguments are required.");
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        // Arguments are not numbers
        printf("[b_xor] Both arguments must be integers");
        return 0;
    }

    lua_Integer a = lua_tointeger(L, 1);
    lua_Integer b = lua_tointeger(L, 2);

    lua_Integer result = a ^ b;

    lua_pushinteger(L, result);
    return 1;
}

/**
 * Performs a binary "not" operation on a single integer.
 *
 * @param L the Lua state.
 */
int b_not(lua_State* L)
{
    if (lua_gettop(L) != 1) {
        // Too few/many arguments passed
        printf("[b_not] One argument is required.");
        return 0;
    }

    if (!lua_isnumber(L, 1)) {
        // Argument is not number
        printf("[b_not] The argument must be an integer");
        return 0;
    }

    lua_Integer a = lua_tointeger(L, 1);

    lua_Integer result = ~a;

    lua_pushinteger(L, result);
    return 1;
}

/**
 * Performs a binary "left shift" operation on an integer.
 *
 * @param L the Lua state.
 */
int b_lshift(lua_State* L)
{
    if (lua_gettop(L) != 2) {
        // Too few/many arguments passed
        printf("[b_lshift] Two arguments are required.");
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        // Arguments are not numbers
        printf("[b_lshift] Both arguments must be integers");
        return 0;
    }

    lua_Integer a = lua_tointeger(L, 1);
    lua_Integer b = lua_tointeger(L, 2);

    lua_Integer result = a << b;

    lua_pushinteger(L, result);
    return 1;
}

/**
 * Performs a binary "right shift" operation on an integer.
 *
 * @param L the Lua state.
 */
int b_rshift(lua_State* L)
{
    if (lua_gettop(L) != 2) {
        // Too few/many arguments passed
        printf("[b_rshift] Two arguments are required.");
        return 0;
    }

    if (!lua_isnumber(L, 1) || !lua_isnumber(L, 2)) {
        // Arguments are not numbers
        printf("[b_rshift] Both arguments must be integers");
        return 0;
    }

    lua_Integer a = lua_tointeger(L, 1);
    lua_Integer b = lua_tointeger(L, 2);

    lua_Integer result = a >> b;

    lua_pushinteger(L, result);
    return 1;
}
