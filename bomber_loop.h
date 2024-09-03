#ifndef BOMBER_LOOP_H
#define BOMBER_LOOP_H

#include <dolphin/dolphin.h>

#include "types/state.h"
#include "types/event.h"
#include "types/point.h"
#include "modules/playerselect.h"
#include "modules/levelselect.h"
#include "modules/gameplay.h"

#include "helpers.h"
#include "subghz/subghz.h"
#include "levels.h"

void bomber_main_loop(BomberAppState* state);

#endif