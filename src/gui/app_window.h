#pragma once

#include "src/gui/welcome_box.h"
#include "src/keybinds/keybinds.h"
#include "src/opts.h"
#include "src/timer.h"
#include <gtk/gtk.h>

#define WINDOW_PAD (8)

typedef struct _LSAppWindowClass {
    GtkApplicationWindowClass parent_class;
} LSAppWindowClass;

typedef struct LSApp {
    GtkApplication parent;
} LSApp;

typedef struct _LSAppClass {
    GtkApplicationClass parent_class;
} LSAppClass;

/**
 * @brief The main LibreSplit application window
 */
typedef struct _LSAppWindow {
    GtkApplicationWindow parent; /*!< The proper GTK base application*/
    char data_path[PATH_MAX]; /*!< The path to the libresplit user config directory */
    ls_game* game;
    ls_timer* timer;
    GdkDisplay* display;
    GtkWidget* container;
    LSWelcomeBox* welcome_box;
    GtkWidget* box;
    GList* components;
    GtkWidget* footer;
    GtkCssProvider* style; // Current style provider, there can be only one
    LSKeybinds keybinds; /*!< The keybinds related to this application window */
    LSOpts opts; /*!< The window options */
} LSAppWindow;

void toggle_decorations(LSAppWindow* win);
void toggle_win_on_top(LSAppWindow* win);

gboolean ls_app_window_resize(GtkWidget* widget, GdkEvent* event, gpointer data);
void ls_app_window_open(LSAppWindow* win, const char* file);

// GTK Forward declaration crud
void ls_app_activate(GApplication* app);
void ls_app_open(GApplication* app, GFile** files, gint n_files, const gchar* hint);

static void ls_app_init(LSApp* self) { }
static void ls_app_class_init(LSAppClass* class)
{
    G_APPLICATION_CLASS(class)->activate = ls_app_activate;
    G_APPLICATION_CLASS(class)->open = ls_app_open;
}

G_DEFINE_TYPE(LSApp, ls_app, GTK_TYPE_APPLICATION)
#define LS_APP_TYPE (ls_app_get_type())

G_DEFINE_TYPE(LSAppWindow, ls_app_window, GTK_TYPE_APPLICATION_WINDOW)
#define LS_APP_WINDOW_TYPE (ls_app_window_get_type())

LSAppWindow* ls_app_window_new(LSApp* app);
static void ls_app_window_class_init(LSAppWindowClass* class)
{
}

void init_window(LSAppWindow* self);

static void ls_app_window_init(LSAppWindow* self)
{
    init_window(self);
}

// Pseudo type-casting macros
#define LS_APP_WINDOW(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), LS_APP_WINDOW_TYPE, LSAppWindow))

#define LS_APP(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), LS_APP_TYPE, LSApp))
