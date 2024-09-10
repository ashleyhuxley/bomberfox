#ifndef HELPERS_H
#define HELPERS_H

#include <stdio.h>
#include <stdlib.h>
#include <furi.h>

#include "types/blocktype.h"
#include "types/state.h"


Player get_player_start_position(uint8_t level[], BlockType blockType);
int ix(int x, int y);
Player* get_current_player(BomberAppState* state);
uint8_t count_bricks_in_level(uint8_t level[]);
void get_random_powerup_locations(uint8_t level[], int n, uint8_t output[]);

#endif