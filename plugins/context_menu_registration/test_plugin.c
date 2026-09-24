#include "plugins/plugin.h"
#include "plugins/plugin_utils.h"
#include <gio/gio.h>
#include <gio/gmenu.h>
#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

const abi_version_t abi_version = 1; // v0.1
const char plugin_name[] = "Test Plugin 4";
const char plugin_description[] = "Does something: just registers a fake plugin context menu item";
const char plugin_version[] = "0.1";
const char plugin_author[] = "The LibreSplit Core Team";

void show_plugin_dialog(GSimpleAction* action, GVariant* parameter, gpointer app)
{
    g_print("This is a test from a plugin\n");
}

int register_context_menu(GMenu* parent, GtkWidget* window)
{
    GSimpleActionGroup* actions = g_simple_action_group_new();
    GSimpleAction* do_print = g_simple_action_new("print_stuff", NULL);
    g_signal_connect(do_print, "activate", G_CALLBACK(show_plugin_dialog), NULL);
    g_action_map_add_action(G_ACTION_MAP(actions), G_ACTION(do_print));
    g_object_unref(do_print);
    gtk_widget_insert_action_group(window, "plugin4", G_ACTION_GROUP(actions));
    g_object_unref(actions);
    g_menu_append(parent, "Print Stuff", "plugin4.print_stuff");
    return 0;
}

int plug_init(PlugAPI* api)
{
    return 0;
}
