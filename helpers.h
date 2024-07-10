#ifndef HELPERS_H
#define HELPERS_H

#include "types.h"

Player bomber_app_get_block(uint8_t level[], BlockType blockType);
int ix(int x, int y);
Player* get_player(BomberAppState* state);
void bomber_app_set_mode(BomberAppState* state, BomberAppMode mode);
uint8_t count_walls(uint8_t level[]);
void get_random_powerup_locations(uint8_t level[], int n, uint8_t output[]);

#endif