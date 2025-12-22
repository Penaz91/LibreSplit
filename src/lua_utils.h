#ifndef __UTILS_H__
#define __UTILS_H__

#include "lua.h"

int shallow_copy_tbl(lua_State* L);
int print_tbl(lua_State* L);
int b_xor(lua_State* L);
int b_and(lua_State* L);
int b_or(lua_State* L);
int b_not(lua_State* L);
int b_lshift(lua_State* L);
int b_rshift(lua_State* L);

#endif /* ifndef __UTILS_H__ */
