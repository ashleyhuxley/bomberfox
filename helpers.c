#include "helpers.h"
#include "types.h"

// Extracts the starting locations of the players from the level data
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

// Helper function to calculate the 1-dimensional index from 2 dimensions
int ix(int x, int y)
{
    return (y * 16) + x;
}

// Returns a pointer to the playable character depending on the isPlayerTwo state
Player* get_player(BomberAppState* state)
{
    if (state->isPlayerTwo)
    {
        return &(state->wolf);
    }
    else
    {
        return &(state->fox);
    }
}

// Set the mode with mutex handling
void bomber_app_set_mode(BomberAppState* state, BomberAppMode mode)
{
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    state->mode = mode;
    furi_mutex_release(state->data_mutex);
}