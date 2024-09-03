#ifndef PLAYERSELECT_H
#define PLAYERSELECT_H

#include <gui/canvas.h>

#include "types/state.h"
#include "types/graphics.h"
#include "modules/modeselect.h"

void bomber_playerselect_render(Canvas* canvas, BomberAppState* state);
bool bomber_playerselect_input(BomberAppState* state, InputEvent input);

#endif