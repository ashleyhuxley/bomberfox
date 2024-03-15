#ifndef BOMBER_TYPES_H
#define BOMBER_TYPES_H

#include <stdint.h>

typedef uint8_t level[8][16];

typedef enum BlockType {
    BlockType_Empty = 0,
    BlockType_Brick = 1,
    BlockType_Bomb = 2,
    BlockType_Player = 3,
    BlockType_Enemy = 4,
    BlockType_Ext = 5,
    BlockType_IrPowerup = 6
} BlockType;

typedef struct {
    uint8_t x;
    uint8_t y;
} Point;

typedef struct {
    uint8_t x;
    uint8_t y;
    uint8_t planted;
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