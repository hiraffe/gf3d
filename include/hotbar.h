#ifndef __HOTBAR__
#define __HOTBAR__

#include "ui.h"

UI* hotbar_new();

void hotbar_open(UI* ui);

void hotbar_close(UI* ui);

void hotbar_think(UI* ui);

void hotbar_draw(UI* ui);

#endif