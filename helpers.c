#include "helpers.h"
#include "types.h"
#include <stdbool.h>

Player bomber_app_get_block(uint8_t level[], BlockType blockType)
{
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if ((BlockType)level[ix(x, y)] == blockType)
            {
                Player player = { x, y };
                return player;
            }
        }
    }

    Player def = { 0, 0 };
    return def;
}

int ix(int x, int y)
{
    return (y * 16) + x;
}