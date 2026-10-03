#pragma once
#include "plugins/plugin_utils.h"
#include <gtk/gtk.h>

// Functions to connect host and plugin
int plug_init(PlugAPI* api);
int plug_shutdown(void);
int register_context_menu(GMenu* parent, GtkWidget* window);
