
#include "glib.h"
#include "gtk/gtk.h"
typedef struct
{
    guint key;
    GdkModifierType mods;
} Keybind;

typedef struct
{
    Keybind start_split;
    Keybind stop_reset;
    Keybind cancel;
    Keybind unsplit;
    Keybind skip_split;
    Keybind toggle_decorations;
    Keybind toggle_win_on_top;
} LSKeybinds;

int keybind_match(Keybind kb, GdkEventKey key);
