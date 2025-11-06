#include "signature.h"
#include "libsigscan.h"
#include "lua.h"
#include "process.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

extern game_process process;

void log_error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Error in sig_scan: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

int perform_sig_scan(lua_State* L)
{
    if (lua_gettop(L) != 1) {
        log_error("Invalid number of arguments: expected 1. (Signature)");
        lua_pushnil(L);
        return 1;
    }

    if (!lua_isstring(L, 1)) {
        log_error("Invalid argument type: expected string");
        lua_pushnil(L);
        return 1;
    }

    pid_t process_pid = process.pid;
    const char* signature = lua_tostring(L, 1);

    if (strlen(signature) == 0) {
        log_error("Signature string cannot be empty.");
        lua_pushnil(L);
        return 1;
    }

    void* ptr = sigscan_pid(process_pid, signature);
    if (ptr == NULL) {
        log_error("Signature not found.");
        lua_pushnil(L);
        return 1;
    }
    char hex_string[32];
    uintptr_t conv_ptr = (uintptr_t)ptr;
    if (snprintf(hex_string, sizeof(hex_string), "0x%lx", conv_ptr) < 0) {
        log_error("Cannot convert result to string.");
        lua_pushnil(L);
        return 1;
    }
    lua_pushstring(L, hex_string);
    return 1;
}
