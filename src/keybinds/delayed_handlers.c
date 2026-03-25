#include "include/keybinds/delayed_handlers.h"
#include "include/gui/app_window.h"
#include "include/keybinds/delayed_callbacks.h"

void process_delayed_handlers(LSAppWindow* win)
{
    if (win->delayed_handlers.stop_reset) {
        timer_stop_or_reset(win);
        win->delayed_handlers.stop_reset = false;
    }
}
