#include "settings_dialog.h"
#include "src/settings/definitions.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>

static void build_settings_dialog(GtkApplication* app, gpointer data)
{
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "LibreSplit Settings");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 600);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_widget_set_margin_top(box, 8);
    gtk_widget_set_margin_bottom(box, 8);
    gtk_widget_set_margin_start(box, 8);
    gtk_widget_set_margin_end(box, 8);
    gtk_widget_set_vexpand(box, TRUE);
    for (size_t s = 0; s < sections_count; ++s) {
        SectionInfo section_info = sections[s];
        GtkWidget* title = gtk_accel_label_new(section_info.name);
        gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(title, GTK_ALIGN_CENTER);
        gtk_container_add(GTK_CONTAINER(box), title);
        for (size_t i = 0; i < section_info.count; ++i) {
            ConfigEntry entry = ((ConfigEntry*)section_info.entries)[i];
            switch (entry.type) {
                case CFG_STRING:
                    GtkWidget* lbl_str = gtk_label_new(entry.desc);
                    gtk_container_add(GTK_CONTAINER(box), lbl_str);
                    GtkWidget* entry_box_str = gtk_entry_new();
                    gtk_container_add(GTK_CONTAINER(box), entry_box_str);
                    break;
                case CFG_INT:
                    GtkWidget* lbl_int = gtk_label_new(entry.desc);
                    gtk_container_add(GTK_CONTAINER(box), lbl_int);
                    GtkWidget* entry_box_int = gtk_entry_new();
                    gtk_container_add(GTK_CONTAINER(box), entry_box_int);
                    break;
                case CFG_BOOL:
                    GtkWidget* checkbox = gtk_check_button_new_with_label(entry.desc);
                    gtk_container_add(GTK_CONTAINER(box), checkbox);
                    break;
            }
        }
    }
    gtk_container_add(GTK_CONTAINER(window), box);
    gtk_widget_show_all(box);
    gtk_window_present(GTK_WINDOW(window));
}

void show_settings_dialog(GSimpleAction* action, GVariant* parameter, gpointer app)
{
    if (parameter != NULL) {
        app = parameter;
    }
    build_settings_dialog(app, NULL);
}
