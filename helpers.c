#include "helpers.h"
#include "types.h"

Player bomber_app_get_player(level* level)
{
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            if ((BlockType)level[x][y] == BlockType_Player)
            {
                Player player = { x, y };
                return player;
            }
        }
    }

    Player def = { 0, 0 };
    return def;
}