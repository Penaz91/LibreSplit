#include "keybinds.h"

int keybind_match(Keybind kb, GdkEventKey key)
{
    return key.keyval == kb.key && kb.mods == (key.state & gtk_accelerator_get_default_mod_mask());
}
