#include "app_window.h"

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
