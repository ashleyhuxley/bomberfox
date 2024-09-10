#ifndef LEVELSELECT_H
#define LEVELSELECT_H

#include "types/state.h"
#include "types/graphics.h"
#include "modules/modeselect.h"

#define ITEMS_PER_PAGE 4

bool bomber_levelselect_input(BomberAppState* state, InputEvent input);
void bomber_levelselect_render(Canvas* canvas, BomberAppState* state);

#endif