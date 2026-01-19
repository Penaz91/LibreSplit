#include "app_window.h"
#include "src/gui/component/components.h"

void toggle_decorations(LSAppWindow* win)
{
    gtk_window_set_decorated(GTK_WINDOW(win), !win->opts.decorated);
    win->opts.decorated = !win->opts.decorated;
}

void toggle_win_on_top(LSAppWindow* win)
{
    gtk_window_set_keep_above(GTK_WINDOW(win), !win->opts.win_on_top);
    win->opts.win_on_top = !win->opts.win_on_top;
}

static void resize_window(LSAppWindow* win,
    int window_width,
    int window_height)
{
    GList* l;
    for (l = win->components; l != NULL; l = l->next) {
        LSComponent* component = l->data;
        if (component->ops->resize) {
            component->ops->resize(component,
                window_width - 2 * WINDOW_PAD,
                window_height);
        }
    }
}

gboolean ls_app_window_resize(GtkWidget* widget,
    GdkEvent* event,
    gpointer data)
{
    LSAppWindow* win = (LSAppWindow*)widget;
    resize_window(win, event->configure.width, event->configure.height);
    return FALSE;
}
