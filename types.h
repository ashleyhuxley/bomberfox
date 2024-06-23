#ifndef BOMBER_TYPES_H
#define BOMBER_TYPES_H

#include <stdint.h>

// A level is basically a 2D array of blocks of one of the following types
typedef enum BlockType {
    BlockType_Empty = 0,
    BlockType_Brick = 1,
    BlockType_Bomb = 2,
    BlockType_Fox = 3,
    BlockType_Wolf = 4,
    BlockType_Exit = 5,
    BlockType_IrPowerup = 6
} BlockType;

// Possible states a bomb can be in
typedef enum BombState {
    BombState_None = 0,         // Bomb not planted yet
    BombState_Planted = 1,      // Bomb has been planted
    BombState_Hot = 2,          // Bomb is about to be detonated
    BombState_Explode = 3       // Bomb has exploded
} BombState;

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

// Represents a bomb. Keeps track of its state and when it was planted.
typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t planted;
    BombState state;
} Bomb;

typedef struct {
    uint8_t x;
    uint8_t y;
} Player;

#endif