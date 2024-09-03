#ifndef MODESELECT_H
#define MODESELECT_H

#include <dolphin/dolphin.h>

#include "levels.h"
#include "types/state.h"
#include "helpers.h"
#include "subghz/subghz.h"

void bomber_app_select_player(BomberAppState* state);
void bomber_app_select_level(BomberAppState* state);
void bomber_app_setup_level(BomberAppState* state);
void bomber_app_start(BomberAppState* state);
void bomber_app_gameover(BomberAppState* state);
void bomber_app_wait(BomberAppState* state);
void bomber_app_error(BomberAppState* state);
void bomber_app_quit(BomberAppState* state);

#endif