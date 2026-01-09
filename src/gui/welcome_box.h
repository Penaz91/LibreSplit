#ifndef WELCOME_BOX_H

#define WELCOME_BOX_H

#include "gtk/gtk.h"

typedef struct {
    GtkWidget* box;
    GtkWidget* welcome_lbl;
} LSWelcomeBox;

LSWelcomeBox* welcome_box_new(GtkWidget* container);
void welcome_box_destroy(LSWelcomeBox* self);

#endif /* end of include guard: WELCOME_BOX_H */
