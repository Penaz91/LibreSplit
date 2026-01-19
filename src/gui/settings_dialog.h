#pragma once

#include "src/settings/definitions.h"
#include <gtk/gtk.h>

extern AppConfig cfg;

void show_settings_dialog(GSimpleAction* action, GVariant* parameter, gpointer app);
