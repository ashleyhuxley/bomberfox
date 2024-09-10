#ifndef PLAYER_H
#define PLAYER_H

#include "types/bomb.h"
#include "settings.h"

typedef struct {
    uint8_t x;
    uint8_t y;
    Bomb bombs[MAX_BOMBS]; // Array of bombs
    uint8_t bomb_power; // Number of blocks a bomb will destroy
    uint8_t bomb_ix; // Index of currently held bomb
    uint8_t bomb_count; // How many bombs the player can use
} Player;

#endif