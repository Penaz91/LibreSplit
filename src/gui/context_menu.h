#pragma once
#include <gtk/gtk.h>

gboolean button_right_click(GtkWidget* widget, GdkEventButton* event, gpointer app);

void open_activated(GSimpleAction* action, GVariant* parameter, gpointer app);
