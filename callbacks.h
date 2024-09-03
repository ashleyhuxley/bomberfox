#ifndef CALLBACKS_H
#define CALLBACKS_H

#include "types/state.h"

#include "modules/gameover.h"
#include "modules/gameplay.h"
#include "modules/levelselect.h"
#include "modules/playerselect.h"
#include "modules/waitforplayer.h"
#include "bomber.h"

void bomber_callback_subghz_read(void* context);
void bomber_callback_ui_render(Canvas* canvas, void* context);
void bomber_callback_ui_input(InputEvent* input_event, void* context_q);
void bomber_callback_timer_tick();

#endif