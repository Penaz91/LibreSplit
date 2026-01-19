#include "welcome_box.h"
#include "utils.h"
#include <gtk/gtk.h>
#include <stdlib.h>

LSWelcomeBox* welcome_box_new(GtkWidget* container)
{
    LSWelcomeBox* self;
    self = malloc(sizeof(LSWelcomeBox));
    if (!self) {
        return NULL;
    }
    self->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_margin_top(self->box, 0);
    add_class(self->box, "welcome-screen");
    gtk_widget_set_margin_bottom(self->box, 0);
    gtk_widget_set_vexpand(self->box, TRUE);
    self->welcome_lbl = gtk_label_new("Welcome to LibreSplit!\nNo split is currently loaded.\nRight click this window to open a split JSON file!");
    gtk_container_add(GTK_CONTAINER(self->box), self->welcome_lbl);
    gtk_container_add(GTK_CONTAINER(container), self->box);
    return self;
}

void welcome_box_destroy(LSWelcomeBox* self)
{
    free(self);
}
