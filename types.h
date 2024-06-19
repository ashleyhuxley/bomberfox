#ifndef BOMBER_TYPES_H
#define BOMBER_TYPES_H

#include <stdint.h>

typedef enum BlockType {
    BlockType_Empty = 0,
    BlockType_Brick = 1,
    BlockType_Bomb = 2,
    BlockType_Player = 3,
    BlockType_Enemy = 4,
    BlockType_Ext = 5,
    BlockType_IrPowerup = 6
} BlockType;

typedef enum BombState {
    BombState_None = 0,
    BombState_Planted = 1,
    BombState_Hot = 2,
    BombState_Explode = 3
} BombState;

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

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

typedef struct {
    int x;
    int y;
} Enemy;

#endif