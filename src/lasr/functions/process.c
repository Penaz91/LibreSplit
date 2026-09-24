#include "process.h"

#include "../utils.h"
#include "src/lasr/maps/maps.h"
#include "src/logging.h"

#include <ctype.h>
#include <dirent.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern atomic_bool auto_splitter_enabled; /*!< Defines if the auto splitter is enabled */

static int get_all_pids_by_name(const char* mode, const char* name, pid_t** output, size_t* output_count)
{
    if (strcmp(mode, "comm") != 0 && strcmp(mode, "cmdline") != 0) {
        LOG_ERRF("Search mode %s not supported", mode);
        return -1;
    }
    *output = NULL;
    *output_count = 0;

    DIR* proc = opendir("/proc");
    if (!proc) {
        LOG_ERR("Cannot open /proc directory");
        return -1;
    }

    pid_t* pids = NULL;
    size_t count = 0;

    struct dirent* entry;

    while ((entry = readdir(proc)) != NULL) {
        const char* dirname = entry->d_name;

        if (*dirname == '\0') {
            // Dirname is just the null terminator, skip
            continue;
        }

        while (*dirname) {
            if (!isdigit((unsigned char)*dirname)) {
                // We found a non-numeric directory name
                break;
            }
            dirname++;
        }

        if (*dirname != '\0') {
            // Dirname didn't reach the null terminator -> non-numeric name -> skip
            continue;
        }

        pid_t pid = (pid_t)strtoul(entry->d_name, NULL, 10);

        char path[256];
        snprintf(path, sizeof(path), "/proc/%u/%s", pid, mode);

        FILE* file = fopen(path, "r");
        if (!file) {
            // The process may have exited during the scan
            continue;
        }

        char comm[256];

        if (fgets(comm, sizeof(comm), file)) {
            // Replace \n with a NUL
            comm[strcspn(comm, "\n")] = '\0';

            if (strcmp(comm, name) == 0) {
                pid_t* new_pids = realloc(pids, (count + 1) * sizeof(*output));
                if (!new_pids) {
                    // Malloc fail, bail out
                    free(pids);
                    free(file);
                    closedir(proc);
                    return -1;
                }

                // Malloc ok, switchover time
                output = &new_pids;
                pids[count++] = pid;
            }
        }
        fclose(file);
    }
    closedir(proc);
    *output = pids;
    *output_count = count;

    return 0;
}

static int compare_pids_ascending(const void* a, const void* b)
{
    const pid_t pid_a = *(const pid_t*)a;
    const pid_t pid_b = *(const pid_t*)b;
    if (pid_a < pid_b) {
        return -1;
    }
    if (pid_a > pid_b) {
        return 1;
    }
    return 0;
}

static int compare_pids_descending(const void* a, const void* b)
{
    const pid_t pid_a = *(const pid_t*)a;
    const pid_t pid_b = *(const pid_t*)b;
    if (pid_a < pid_b) {
        return -1;
    }
    if (pid_a > pid_b) {
        return 1;
    }
    return 0;
}

static pid_t get_pid(const char* mode, const char* sort, const char* name)
{
    if (strcmp(mode, "comm") != 0 && strcmp(mode, "cmdline") != 0) {
        LOG_ERRF("Search mode %s not supported", mode);
        return NULL;
    }
    if (strcmp(sort, "first") != 0 && strcmp(sort, "last") != 0) {
        LOG_ERRF("Search mode %s not supported", mode);
        printf("[process] Invalid sort argument '%s'. Use 'first' or 'last'. Falling back to first\n", sort);
        sort = "first";
    }

    pid_t* pids = NULL;
    size_t count = 0;
    get_all_pids_by_name(mode, name, &pids, &count);

    if (!pids) {
        return 0;
    }

    if (strcmp(sort, "first") == 0) {
        qsort(pids, count, sizeof(pids[0]), compare_pids_ascending);
    }
    if (strcmp(sort, "last") == 0) {
        qsort(pids, count, sizeof(pids[0]), compare_pids_descending);
    }

    pid_t result = pids[0];
    free(pids);
    pids = NULL;
    count = 0;
    return result;
}

/**
 * Executes a command, piping its output into an output string.
 *
 * @param command The command to execute.
 * @param output Pointer to a string that will contain the command output.
 */
void execute_command(const char* command, char* output)
{
    char buffer[4096];
    FILE* pipe = popen(command, "r");
    if (!pipe) {
        fprintf(stderr, "Error executing command: %s\n", command);
        exit(1);
    }

    while (fgets(buffer, 128, pipe) != NULL) {
        strcat(output, buffer);
    }

    pclose(pipe);
}

void stock_process_id(const char* pid_command)
{
    char pid_output[PATH_MAX + 100];
    pid_output[0] = '\0';

    // We just started a new process monitoring, we may want to clean up a stale cache
    maps_clearCache();

    while (atomic_load(&auto_splitter_enabled)) {
        execute_command(pid_command, pid_output);
        process.pid = strtoul(pid_output, NULL, 10);
        if (process.pid) {
            size_t newlinePos = strcspn(pid_output, "\n");
            if (newlinePos != strlen(pid_output) - 1 && pid_output[0] != '\0') {
                printf("Multiple PID's found for process: %s\n", process.name);
            }
            break;
        } else {
            printf("%s isn't running.\n", process.name);
            usleep(100000); // Sleep for 100ms
        }
    }

    printf("Process: %s\n", process.name);
    printf("PID: %u\n", process.pid);
    process.base_address = find_base_address(NULL);
    process.dll_address = process.base_address;
}

/**
 * Finds the ID of the process indicated by the Lua Auto Splitter.
 *
 * @param L The Lua State.
 *
 * @return Always zero.
 */
int find_process_id(lua_State* L)
{
    printf("\033[2J\033[1;1H"); // Clear the console

    process.name = lua_tostring(L, 1);
    const char* sort = lua_tostring(L, 2);
    char sortCmd[16] = "";

    if (!sort) {
        sort = "first";
    } else {
        if (strcmp(sort, "first") != 0 && strcmp(sort, "last") != 0) {
            printf("[process] Invalid sort argument '%s'. Use 'first' or 'last'. Falling back to first\n", sort);
            sort = "first";
        }
    }

    if (strcmp(sort, "first") == 0) {
        sortCmd[0] = '\0'; // No sorting
    }
    if (strcmp(sort, "last") == 0) {
        strcpy(sortCmd, " | sort -r"); // Reverse the sorting to get latest PID
    }

    char command[256];
    snprintf(command, sizeof(command), "pgrep \"%.*s\"%s", (int)strnlen(process.name, sizeof(command) - strlen(sortCmd) - 1), process.name, sortCmd);

    stock_process_id(command);

    return 0;
}

/**
 * Finds the ID of the process indicated by the Lua Auto Splitter using full commandline grepping.
 *
 *  NOTE: [Penaz] [2026-04-25] This differs from find_process_id only by the -f argument. Consider
 *  ^ merging the command creation into a single function instead of duplicating code.
 *
 * @param L The Lua State.
 *
 * @return Always zero.
 */
int find_cmdline_id(lua_State* L)
{
    printf("\033[2J\033[1;1H"); // Clear the console

    process.name = lua_tostring(L, 1);
    const char* sort = lua_tostring(L, 2);
    char sortCmd[16] = "";

    if (!sort) {
        sort = "first";
    } else {
        if (strcmp(sort, "first") != 0 && strcmp(sort, "last") != 0) {
            printf("[process] Invalid sort argument '%s'. Use 'first' or 'last'. Falling back to first\n", sort);
            sort = "first";
        }
    }

    if (strcmp(sort, "first") == 0) {
        sortCmd[0] = '\0'; // No sorting
    }
    if (strcmp(sort, "last") == 0) {
        strcpy(sortCmd, " | sort -r"); // Reverse the sorting to get latest PID
    }

    char command[256];
    snprintf(command, sizeof(command), "pgrep -f \"%.*s\"%s", (int)strnlen(process.name, sizeof(command) - strlen(sortCmd) - 1), process.name, sortCmd);

    stock_process_id(command);

    return 0;
}
