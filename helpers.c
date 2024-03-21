#include "helpers.h"
#include "types.h"
#include <stdbool.h>

Player bomber_app_get_player(uint8_t level[])
{
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if ((BlockType)level[ix(x, y)] == BlockType_Player)
            {
                level[ix(x, y)] = (uint8_t)BlockType_Empty;
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