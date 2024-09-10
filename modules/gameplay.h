#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "types/state.h"
#include "types/graphics.h"
#include "types/blocktype.h"
#include "helpers.h"
#include "types/point.h"
#include "subghz/subghz.h"

void bomber_game_render(Canvas* canvas, BomberAppState* state);
bool bomber_game_input(BomberAppState* state, InputEvent input);
bool bomber_game_tick(BomberAppState* state);
void bomber_game_player_move(BomberAppState* state, Player* player, uint8_t x, uint8_t y);

#endif