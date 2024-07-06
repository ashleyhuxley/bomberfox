#ifndef SUBGHZ_H
#define SUBGHZ_H

#include "types.h"

#define MESSAGE_COMPLETION_TIMEOUT 500

void tx_new_position(Player* player, BomberAppState* state);
void tx_bomb_placement(BomberAppState* state, uint8_t x, uint8_t y);
void subghz_check_incoming(BomberAppState* state);
void have_read_cb(void* context);

#endif