#include "settings_dialog.h"
#include "src/settings/definitions.h"
#include <gdk-pixbuf/gdk-pixbuf.h>
#include <gtk/gtk.h>
#include <stdio.h>

static void build_settings_dialog(GtkApplication* app, gpointer data)
{
    GtkWidget* window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "LibreSplit Settings");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 500);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    GtkWidget* tabs = gtk_notebook_new();
    gtk_widget_set_margin_top(tabs, 8);
    gtk_widget_set_margin_bottom(tabs, 8);
    gtk_widget_set_margin_start(tabs, 8);
    gtk_widget_set_margin_end(tabs, 8);
    gtk_widget_set_vexpand(tabs, TRUE);
    gtk_widget_set_hexpand(tabs, TRUE);
    for (size_t s = 0; s < sections_count; ++s) {
        SectionInfo section_info = sections[s];
        if (!section_info.in_gui) {
            continue;
        }
        GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
        gtk_widget_set_margin_top(box, 8);
        gtk_widget_set_margin_bottom(box, 8);
        gtk_widget_set_margin_start(box, 8);
        gtk_widget_set_margin_end(box, 8);
        gtk_widget_set_vexpand(box, TRUE);
        gtk_widget_set_hexpand(box, TRUE);
        GtkWidget* title = gtk_accel_label_new(section_info.name);
        for (size_t i = 0; i < section_info.count; ++i) {
            ConfigEntry entry = ((ConfigEntry*)section_info.entries)[i];
            switch (entry.type) {
                case CFG_STRING:
                    GtkWidget* lbl_str = gtk_label_new(entry.desc);
                    gtk_container_add(GTK_CONTAINER(box), lbl_str);
                    GtkEntryBuffer* entry_box_buffer_str = gtk_entry_buffer_new(entry.value.s, sizeof(entry.value.s));
                    GtkWidget* entry_box_str = gtk_entry_new_with_buffer(entry_box_buffer_str);
                    gtk_container_add(GTK_CONTAINER(box), entry_box_str);
                    break;
                case CFG_KEYBIND:
                    GtkWidget* lbl_kb = gtk_label_new(entry.desc);
                    gtk_container_add(GTK_CONTAINER(box), lbl_kb);
                    GtkEntryBuffer* entry_box_buffer_kb = gtk_entry_buffer_new(entry.value.s, sizeof(entry.value.s));
                    GtkWidget* entry_box_kb = gtk_entry_new_with_buffer(entry_box_buffer_kb);
                    gtk_container_add(GTK_CONTAINER(box), entry_box_kb);
                    break;
                case CFG_BOOL:
                    GtkWidget* checkbox = gtk_check_button_new_with_label(entry.desc);
                    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(checkbox), entry.value.b);
                    gtk_container_add(GTK_CONTAINER(box), checkbox);
                    break;
                case CFG_INT:
                    GtkWidget* lbl_int = gtk_label_new(entry.desc);
                    gtk_container_add(GTK_CONTAINER(box), lbl_int);
                    char setting_as_str[(int)((entry.value.i / 10) + 1)];
                    sprintf(setting_as_str, "%d", entry.value.i);
                    GtkEntryBuffer* entry_box_buffer_int = gtk_entry_buffer_new(setting_as_str, sizeof(setting_as_str));
                    GtkWidget* entry_box_int = gtk_entry_new_with_buffer(entry_box_buffer_int);
                    gtk_container_add(GTK_CONTAINER(box), entry_box_int);
                    break;
            }
        }
        gtk_notebook_append_page(GTK_NOTEBOOK(tabs), box, title);
    }
    gtk_container_add(GTK_CONTAINER(window), tabs);
    gtk_widget_show_all(tabs);
    gtk_window_present(GTK_WINDOW(window));
}

void show_settings_dialog(GSimpleAction* action, GVariant* parameter, gpointer app)
{
    if (parameter != NULL) {
        app = parameter;
    }
    build_settings_dialog(app, NULL);
}
