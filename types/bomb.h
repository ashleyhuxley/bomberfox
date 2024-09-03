#ifndef BOMB_H
#define BOMB_H

#include <stdint.h>

// Possible states a bomb can be in
typedef enum BombState {
    BombState_None = 0, // Bomb not planted yet
    BombState_Planted = 1, // Bomb has been planted
    BombState_Hot = 2, // Bomb is about to be detonated
    BombState_Explode = 3 // Bomb has exploded
} BombState;

// Represents a bomb. Keeps track of its state and when it was planted.
typedef struct {
    uint8_t x;
    uint8_t y;
    uint32_t planted;
    BombState state;
} Bomb;

#endif