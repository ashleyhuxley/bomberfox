#ifndef SUBGHZ_H
#define SUBGHZ_H

#include "types.h"

#define MESSAGE_COMPLETION_TIMEOUT 500

void tx_new_position(Player* player, BomberAppState* state);
void subghz_check_incoming(BomberAppState* state);

#endif