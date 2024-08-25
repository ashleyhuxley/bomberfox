#include "helpers.h"
#include "types.h"

#include <stdio.h>
#include <stdlib.h>
#include <furi.h>

// Function to count the number of walls in a given level
uint8_t count_walls(uint8_t level[])
{
    uint8_t brickCount = 0;
    for (uint8_t i = 0; i < LEVEL_SIZE; i++) {
        if (level[i] == BlockType_Brick) {
            brickCount++;
        }
    }

    return brickCount;
}

// Function to select (n) random walls from the level data. Returns the indicies in the output array
void get_random_powerup_locations(uint8_t level[], int n, uint8_t output[]) {
    uint8_t brickCount = count_walls(level);

    srand(furi_get_tick());

    // Store the indices of all BlockType_Brick elements
    uint8_t *brickIndices = (uint8_t *)malloc(brickCount * sizeof(uint8_t));
    if (brickIndices == NULL) {
        FURI_LOG_E(TAG, "Memory allocation failed.");
        return;
    }

    uint8_t index = 0;
    for (uint8_t i = 0; i < LEVEL_SIZE; i++) {
        if (level[i] == BlockType_Brick) {
            brickIndices[index++] = i;
        }
    }

    // Fisher-Yates shuffle algorithm to shuffle the brickIndices array
    for (uint8_t i = brickCount - 1; i > 0; i--) {
        uint8_t j = rand() % (i + 1);
        uint8_t temp = brickIndices[i];
        brickIndices[i] = brickIndices[j];
        brickIndices[j] = temp;
    }

    // Copy the first n elements from the shuffled brickIndices to the output array
    for (uint8_t i = 0; i < n; i++) {
        output[i] = brickIndices[i];
    }

    free(brickIndices);
}

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
                    .bomb_power = 1,
                    .bomb_count = 1
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
        .bomb_power = 1,
        .bomb_count = 1
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

bool is_solid_block(BlockType type) {
    return type == BlockType_Brick ||
           type == BlockType_PuBombStrength_Hidden ||
           type == BlockType_PuExtraBomb_Hidden;
}