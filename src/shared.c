#include "shared.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void getXDGruntimeDir(char* buffer, size_t size)
{
    const char* xdg_runtime_dir = getenv("XDG_RUNTIME_DIR");
    buffer[0] = '\0';
    if (xdg_runtime_dir) {
        strncpy(buffer, xdg_runtime_dir, size - 1);
        buffer[size - 1] = '\0';
        return;
    }

    const int uid = getuid();
    snprintf(buffer, size, "/run/user/%d", uid);
}
