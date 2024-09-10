#ifndef BLOCKTYPE_H
#define BLOCKTYPE_H

// A level is basically a 2D array of blocks of one of the following types
typedef enum BlockType {
    BlockType_Empty = 0,
    BlockType_Brick = 1,
    BlockType_Bomb = 2,
    BlockType_Fox = 3,
    BlockType_Wolf = 4,
    BlockType_PuExtraBomb = 5,
    BlockType_PuBombStrength = 6,
    BlockType_PuExtraBomb_Hidden = 7,
    BlockType_PuBombStrength_Hidden = 8,
    BlockType_Wall = 9
} BlockType;

#endif