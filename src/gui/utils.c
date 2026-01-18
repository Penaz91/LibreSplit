#include <gtk/gtk.h>

/**
 * Adds a styling class to a GTK Widget.
 *
 * @param widget The widget to add the class to
 * @param class The class to add
 */
void add_class(GtkWidget* widget, const char* class)
{
    gtk_style_context_add_class(gtk_widget_get_style_context(widget), class);
}

/**
 * Removes a styling class from a GTK Widget.
 *
 * @param widget The widget to remove the class from
 * @param class The class to remove
 */
void remove_class(GtkWidget* widget, const char* class)
{
    gtk_style_context_remove_class(gtk_widget_get_style_context(widget), class);
}
