#ifndef HELPERS_H
#define HELPERS_H

#include <stdbool.h>
#include "types.h"
#include "bomber.h"

Player bomber_app_get_block(uint8_t level[], BlockType blockType);
int ix(int x, int y);
Player* get_player(BomberAppState* state);

#endif