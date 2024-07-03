#include "helpers.h"
#include "types.h"

// Extracts the starting location of the player from the level data
// Returns the player's position if found, otherwise returns a default position (0,0)
Player bomber_app_get_block(uint8_t level[], BlockType blockType)
{
    for(int x = 0; x < 16; x++)
    {
        for(int y = 0; y < 8; y++)
        {
            if((BlockType)level[ix(x, y)] == blockType)
            {
                Player player = {
                    .x = x,
                    .y = y,
                    .bombs = { [0 ... 9] = {0, 0, 0, BombState_None} },  // All elements initialized the same way
                    .bomb_power = 1
                };
                return player;
            }
        }
    }

    // Log a warning if the block type is not found in the level data
    FURI_LOG_W(TAG, "Block type %d not found in level data", blockType);

    // Return a default position if block type is not found
    Player def = {
        .x = 0,
        .y = 0,
        .bombs = { [0 ... 9] = {0, 0, 0, BombState_None} },  // All elements initialized the same way
        .bomb_power = 1
    };
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
    furi_assert(state);
    return state->isPlayerTwo ? &(state->wolf) : &(state->fox);
}


// Set the mode with mutex handling
void bomber_app_set_mode(BomberAppState* state, BomberAppMode mode)
{
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    state->mode = mode;
    furi_mutex_release(state->data_mutex);
}