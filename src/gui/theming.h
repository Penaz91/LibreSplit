#pragma once

#include "src/gui/app_window.h"

static const unsigned char fallback_css_data[] = {
#embed "../fallback.css"
};

static const size_t fallback_css_data_len = sizeof(fallback_css_data);

int ls_app_window_find_theme(const LSAppWindow* win, const char* name, const char* variant, char* out_path);

void ls_app_load_theme_with_fallback(LSAppWindow* win, const char* name, const char* variant);
