#include "keybinds.h"

int keybind_match(Keybind kb, GdkEventKey key)
{
    return key.keyval == kb.key && kb.mods == (key.state & gtk_accelerator_get_default_mod_mask());
}

Keybind parse_keybind(const gchar* accelerator)
{
    Keybind kb;
    gtk_accelerator_parse(accelerator, &kb.key, &kb.mods);
    return kb;
}
