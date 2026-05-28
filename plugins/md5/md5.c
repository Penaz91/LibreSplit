#include "lua.h"
#include "plugins/plugin.h"
#include "plugins/plugin_utils.h"
#include <openssl/md5.h>
#include <stdio.h>

const abi_version_t abi_version = 0 << 16 | 1; // v0.1
const char plugin_name[] = "MD5 Function Plugin";
const char plugin_description[] = "Adds a Lua function to calculate the MD5 checksum of a file, given a path";
const char plugin_version[] = "0.1";
const char plugin_author[] = "The LibreSplit Core Team";

int md5sum(lua_State* L)
{
    if (lua_gettop(L) != 1) {
        printf("This function requires the path to a file to calculate the MD5 checksum");
        lua_pushnil(L);
        return 1;
    }
    if (!lua_isstring(L, 1)) {
        printf("The argument must be a string");
        lua_pushnil(L);
        return 1;
    }
    const char* file_path = lua_tostring(L, 1);
    MD5_CTX c;
    char buffer[512];
    ssize_t bytes;
    unsigned char md5_out[MD5_DIGEST_LENGTH];
    FILE* file_ptr = NULL;
    file_ptr = fopen(file_path, "r");
    if (file_ptr == NULL) {
        printf("Error while opening the file for MD5 hashing");
        lua_pushnil(L);
        return 1;
    }
    // FIXME: [Penaz] [2026-05-27] This method is deprecated, might not work on some systems
    // Should probably be updated to the EVP Digest API
    MD5_Init(&c);
    do {
        bytes = fread(buffer, 1, 512, file_ptr);
        MD5_Update(&c, buffer, bytes);
    } while (bytes > 0);
    MD5_Final(md5_out, &c);
    int pos = 0;
    char output[MD5_DIGEST_LENGTH * 2 + 1];
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        pos += snprintf(output + pos, sizeof(output) - pos, "%02x", md5_out[i]);
    }
    lua_pushstring(L, output);
    fclose(file_ptr);
    return 1;
}

int plug_init(PlugAPI* api)
{
    api->register_lua_function("md5sum", md5sum);
    return 0;
}
