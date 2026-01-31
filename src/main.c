#include "gui/app_window.h"
#include "gui/context_menu.h"
#include "gui/settings_dialog.h"
#include "gui/timer.h"
#include "keybinds/keybinds_callbacks.h"
#include "lasr/auto-splitter.h"
#include "server.h"
#include "settings/settings.h"
#include "settings/utils.h"
#include "shared.h"
#include "timer.h"

#include <gtk/gtk.h>
#include <jansson.h>
#include <linux/limits.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/prctl.h>
#include <sys/stat.h>

atomic_bool exit_requested = 0; /*!< Set to 1 when LibreSplit is exiting */

// Global application instance for CTL command handling
static LSApp* g_app = NULL;

// Function to handle CTL commands from the server thread
void handle_ctl_command(CTLCommand command)
{
    GList* windows;
    LSAppWindow* win;

    if (!g_app) {
        printf("No application instance available to handle command\n");
        return;
    }

    windows = gtk_application_get_windows(GTK_APPLICATION(g_app));
    if (windows) {
        win = LS_APP_WINDOW(windows->data);
    } else {
        printf("No window available to handle command\n");
        return;
    }

    switch (command) {
        case CTL_CMD_START_SPLIT:
            timer_start_split(win);
            break;
        case CTL_CMD_STOP_RESET:
            timer_stop_reset(win);
            break;
        case CTL_CMD_CANCEL:
            timer_cancel_run(win);
            break;
        case CTL_CMD_UNSPLIT:
            timer_unsplit(win);
            break;
        case CTL_CMD_SKIP:
            timer_skip(win);
            break;
        case CTL_CMD_EXIT:
            exit(0);
            break;
        default:
            printf("Unknown CTL command: %d\n", command);
            break;
    }
}

/**
 * Starts LibreSplit, loading the last splits and auto splitter.
 * Eventually opens some dialogs if there are no last splits or auto-splitters.
 *
 * @param app Pointer to the LibreSplit application.
 */
void ls_app_activate(GApplication* app)
{
    if (!config_init()) {
        printf("Configuration failed to load, will use defaults\n");
    }

    LSAppWindow* win;
    win = ls_app_window_new(LS_APP(app));
    gtk_window_present(GTK_WINDOW(win));

    if (cfg.history.split_file.value.s[0] != '\0') {
        // Check if split file exists
        struct stat st = { 0 };
        char splits_path[PATH_MAX];
        strcpy(splits_path, cfg.history.split_file.value.s);
        if (stat(splits_path, &st) == -1) {
            printf("Split JSON %s does not exist\n", splits_path);
            open_activated(NULL, NULL, app);
        } else {
            ls_app_window_open(win, splits_path);
        }
    } else {
        open_activated(NULL, NULL, app);
    }
    if (cfg.history.auto_splitter_file.value.s[0] != '\0') {
        struct stat st = { 0 };
        char auto_splitters_path[PATH_MAX];
        strcpy(auto_splitters_path, cfg.history.auto_splitter_file.value.s);
        if (stat(auto_splitters_path, &st) == -1) {
            printf("Auto Splitter %s does not exist\n", auto_splitters_path);
        } else {
            strcpy(auto_splitter_file, auto_splitters_path);
        }
    }
    atomic_store(&auto_splitter_enabled, cfg.libresplit.auto_splitter_enabled.value.b);
    g_signal_connect(win, "button_press_event", G_CALLBACK(button_right_click), app);
}

void ls_app_open(GApplication* app,
    GFile** files,
    gint n_files,
    const gchar* hint)
{
    GList* windows;
    LSAppWindow* win;
    int i;
    windows = gtk_application_get_windows(GTK_APPLICATION(app));
    if (windows) {
        win = LS_APP_WINDOW(windows->data);
    } else {
        win = ls_app_window_new(LS_APP(app));
    }
    for (i = 0; i < n_files; i++) {
        ls_app_window_open(win, g_file_get_path(files[i]));
    }
    gtk_window_present(GTK_WINDOW(win));
}

LSApp* ls_app_new(void)
{
    g_set_application_name("LibreSplit");
    return g_object_new(LS_APP_TYPE,
        "application-id", "com.github.wins1ey.libresplit",
        "flags", G_APPLICATION_HANDLES_OPEN,
        NULL);
}

/**
 * LibreSplit's auto splitter thread.
 *
 * @param arg Unused.
 */
static void* ls_auto_splitter(void* arg)
{
    prctl(PR_SET_NAME, "LS LASR", 0, 0, 0);
    while (1) {
        if (atomic_load(&auto_splitter_enabled) && auto_splitter_file[0] != '\0') {
            atomic_store(&auto_splitter_running, true);
            run_auto_splitter();
        }
        atomic_store(&auto_splitter_running, false);
        if (atomic_load(&exit_requested))
            return 0;
        usleep(50000);
    }
    return NULL;
}

int main(int argc, char* argv[])
{
    check_directories();

    g_app = ls_app_new();
    pthread_t t1; // Auto-splitter thread
    pthread_create(&t1, NULL, &ls_auto_splitter, NULL);

    pthread_t t2; // Control server thread
    pthread_create(&t2, NULL, &ls_ctl_server, NULL);

    g_application_run(G_APPLICATION(g_app), argc, argv);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
