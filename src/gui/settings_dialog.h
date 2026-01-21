#pragma once

#include "src/settings/definitions.h"
#include <gtk/gtk.h>

extern AppConfig cfg;

typedef struct LSGuiSetting {
    GtkWidget* widget;
    GtkEntryBuffer* entry_buffer;
    ConfigEntry* settings_entry;
} LSGuiSetting;

void show_settings_dialog(GSimpleAction* action, GVariant* parameter, gpointer app);
