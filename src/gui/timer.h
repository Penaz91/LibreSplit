#pragma once

#include "src/gui/app_window.h"

void timer_reset(LSAppWindow* win);
void timer_start_split(LSAppWindow* win);
void timer_stop_reset(LSAppWindow* win);
void timer_unsplit(LSAppWindow* win);
void timer_skip(LSAppWindow* win);
void timer_stop(LSAppWindow* win);
void timer_split(LSAppWindow* win, bool updateComponents);
