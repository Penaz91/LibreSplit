/** \file comparison.c
 *
 * Implementation of the "Comparing against..." component
 */
#include "components.h"

/**
 * @brief The component representing the current type of comparison
 */
typedef struct LSComparison {
    LSComponent base; /*!< The base struct that is extended */
    GtkWidget* container; /*!< The container for the comparison labels */
    GtkWidget* comparison_label; /*!< The label showing the "Comparing against" text */
    GtkWidget* comparison; /*!< The label showing the current selected comparison */
} LSComparison;
extern LSComponentOps ls_comparison_operations;

#define comparison_str "Comparing Against"

/**
 * Constructor
 */
LSComponent* ls_component_comparison_new(void)
{
    LSComparison* self;

    self = malloc(sizeof(LSComparison));
    if (!self) {
        return NULL;
    }
    self->base.ops = &ls_comparison_operations;

    self->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    add_class(self->container, "footer"); /* hack */
    add_class(self->container, "comparison-container");
    gtk_widget_show(self->container);

    self->comparison_label = gtk_label_new(comparison_str);
    add_class(self->comparison_label, "comparison-label");
    gtk_container_add(GTK_CONTAINER(self->container), self->comparison_label);

    self->comparison = gtk_label_new(NULL);
    add_class(self->comparison, "comparison");
    gtk_widget_set_halign(self->comparison, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(self->container), self->comparison);

    return (LSComponent*)self;
}

/**
 * Destructor
 *
 * @param self The component to destroy
 */
static void comparison_delete(LSComponent* self)
{
    free(self);
}

/**
 * Returns the Comparison GTK widget.
 *
 * @param self The Comparison component itself.
 * @return The container as a GTK Widget.
 */
static GtkWidget* comparison_widget(LSComponent* self)
{
    return ((LSComparison*)self)->container;
}

/**
 * Function to execute when ls_app_window_show_game is executed.
 *
 * @param self_ The Comparison component itself.
 * @param game The game struct instance.
 * @param timer The timer instance.
 */
static void comparison_show_game(LSComponent* self_,
    const ls_game* game, const ls_timer* timer)
{
    LSComparison* self = (LSComparison*)self_;
    gtk_widget_set_halign(self->comparison_label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(self->comparison_label, TRUE);
    gtk_widget_show(self->comparison);
    gtk_widget_show(self->comparison_label);
}

/**
 * Function to execute when ls_app_window_clear_game is executed.
 *
 * @param self_ The best time component itself.
 */
static void comparison_clear_game(LSComponent* self_)
{
    LSComparison* self = (LSComparison*)self_;
    gtk_widget_hide(self->comparison_label);
    gtk_widget_hide(self->comparison);
}

/**
 * Function to execute when ls_app_window_draw is executed.
 *
 * @param self_ The Comparison component itself.
 * @param game The game struct instance.
 * @param timer The timer instance.
 */
static void comparison_draw(LSComponent* self_, const ls_game* game,
    const ls_timer* timer)
{
    LSComparison* self = (LSComparison*)self_;
    // NOTE: [Penaz] [2026-03-01] Placeholder
    gtk_label_set_text(GTK_LABEL(self->comparison), "Personal Best");
}

LSComponentOps ls_comparison_operations = {
    .delete = comparison_delete,
    .widget = comparison_widget,
    .show_game = comparison_show_game,
    .clear_game = comparison_clear_game,
    .draw = comparison_draw
};
