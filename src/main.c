#include "gui/app_window.h"
#include "gui/timer.h"
#include "keybinds/delayed_callbacks.h"
#include "keybinds/keybinds_callbacks.h"
#include "lasr/auto-splitter.h"
#include "logging.h"
#include "plugins/plugin_loading.h"
#include "plugins/plugin_utils.h"
#include "server.h"
#include "settings/utils.h"
#include "shared.h"
#include "timer.h"

#include <gtk/gtk.h>
#include <jansson.h>
#include <linux/limits.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/prctl.h>
#include <sys/stat.h>

atomic_bool exit_requested = 0; /*!< Set to 1 when LibreSplit is exiting */

// Global application instance for CTL command handling
static LSApp* g_app = NULL;

// Function to handle CTL commands from the server thread
void handle_ctl_command(const char* command)
{
    GList* windows;
    LSAppWindow* win;

    if (!g_app) {
        LOG_INFO("No application instance available to handle commands");
        return;
    }

    windows = gtk_application_get_windows(GTK_APPLICATION(g_app));
    if (windows) {
        win = LS_APP_WINDOW(windows->data);
    } else {
        LOG_INFO("No window available to handle commands");
        return;
    }

    if (strcmp(command, "startorsplit") == 0) {
        timer_start_split(win);
    } else if (strcmp(command, "stoporreset") == 0) {
        timer_stop_or_reset(win);
    } else if (strcmp(command, "cancel") == 0) {
        timer_cancel_run(win);
    } else if (strcmp(command, "unsplit") == 0) {
        timer_unsplit(win);
    } else if (strcmp(command, "skipsplit") == 0) {
        timer_skip(win);
    } else if (strcmp(command, "exit") == 0) {
        exit(0);
    } else {
        printf("Unknown CTL command: %s\n", command);
    }
}

/**
 * LibreSplit's auto splitter thread.
 *
 * @param arg Unused.
 */
static void* ls_auto_splitter(void* arg)
{
    prctl(PR_SET_NAME, "LS LASR", 0, 0, 0);
    init_lasr_functions();
    while (1) {
        if (atomic_load(&auto_splitter_enabled) && auto_splitter_file[0] != '\0') {
            atomic_store(&auto_splitter_running, true);
            run_auto_splitter();
        }
        atomic_store(&auto_splitter_running, false);
        if (atomic_load(&exit_requested)) {
            LOG_DEBUG("Exit requested, shutting down Auto Splitter Thread");
            return 0;
        }
        usleep(50000);
    }
    unregister_luac_functions();
    return NULL;
}

int main(int argc, char* argv[])
{
    initLogQueue();
    LOG_INFOF("Starting LibreSplit - version %s", APP_VERSION);
    check_directories();
    init_external_lasr_functions();
    init_timer_registries();
    load_plugins();

    g_app = ls_app_new();
    LOG_INFO("Creating Auto-Splitter Thread");
    pthread_t t1; // Auto-splitter thread
    pthread_create(&t1, NULL, &ls_auto_splitter, NULL);

    LOG_INFO("Creating Control Server Thread");
    pthread_t t2; // Control server thread
    pthread_create(&t2, NULL, &ls_ctl_server, NULL);

    LOG_INFO("Creating Log Consumer Thread");
    pthread_t t3; // Logging Thread
    pthread_create(&t3, NULL, &loggingThread, NULL);

    g_application_run(G_APPLICATION(g_app), argc, argv);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    return 0;
}
