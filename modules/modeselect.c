#include "modules/modeselect.h"

// Set the mode with mutex handling
void bomber_app_set_mode(BomberAppState* state, BomberAppMode mode)
{
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    state->mode = mode;
    furi_mutex_release(state->data_mutex);
}

// Select the player
void bomber_app_select_player(BomberAppState* state) {
    FURI_LOG_I(TAG, "Selecting Player");
    bomber_app_set_mode(state, BomberAppMode_PlayerSelect);
}

// Quit the app
void bomber_app_quit(BomberAppState* state) {
    FURI_LOG_I(TAG, "Quitting");
    bomber_app_set_mode(state, BomberAppMode_Quit);
}

// Put the game in error state
void bomber_app_error(BomberAppState* state) {
    FURI_LOG_E(TAG, "Error occurred");
    bomber_app_set_mode(state, BomberAppMode_Error);
}

// Wait for Player 1 to select level
void bomber_app_wait(BomberAppState* state) {
    FURI_LOG_E(TAG, "Waiting for P1 to select level");
    state->rxMode = RxMode_LevelData;
    bomber_app_set_mode(state, BomberAppMode_Waiting);
}

// Show the Game Over screen
void bomber_app_gameover(BomberAppState* state) {
    FURI_LOG_E(TAG, "Game Over");
    bomber_app_set_mode(state, BomberAppMode_GameOver);
}

// Start playing
void bomber_app_start(BomberAppState* state) {
    FURI_LOG_I(TAG, "Start playing");

    dolphin_deed(DolphinDeedPluginGameStart);

    // Figure out player starting positions from level data
    state->fox = get_player_start_position(state->level, BlockType_Fox);
    state->level[ix(state->fox.x, state->fox.y)] = (uint8_t)BlockType_Empty;
    state->wolf = get_player_start_position(state->level, BlockType_Wolf);
    state->level[ix(state->wolf.x, state->wolf.y)] = (uint8_t)BlockType_Empty;

    bomber_app_set_mode(state, BomberAppMode_Playing);
    //furi_timer_start(state->timer, furi_ms_to_ticks(50));
}

// Fill level with powerups
void bomber_app_setup_level(BomberAppState* state) {
    state->level = all_levels[state->selectedLevel];
    uint8_t brick_count = count_bricks_in_level(state->level);
    uint8_t powerup_bomb_count = (uint8_t)round((POWERUP_EXTRABOMB_RATIO * brick_count));
    uint8_t powerup_power_count = (uint8_t)round((POWERUP_BOMBPOWER_RATIO * brick_count));
    FURI_LOG_D(TAG, "Walls: %d, Extra Bombs: %d, Bomb Power: %d", brick_count, powerup_bomb_count, powerup_power_count);

    uint8_t* bomb_powerups = malloc(sizeof(uint8_t) * powerup_bomb_count);
    uint8_t* power_powerups = malloc(sizeof(uint8_t) * powerup_power_count);

    get_random_powerup_locations(state->level, powerup_bomb_count, bomb_powerups);
    get_random_powerup_locations(state->level, powerup_power_count, power_powerups);

    for (uint8_t i = 0; i < powerup_bomb_count; i++) {
        state->level[bomb_powerups[i]] = BlockType_PuExtraBomb_Hidden;
    }
    for (uint8_t i = 0; i < powerup_power_count; i++) {
        state->level[power_powerups[i]] = BlockType_PuBombStrength_Hidden;
    }

    free(bomb_powerups);
    free(power_powerups);

    // Tx level data
    subghz_tx_level_data(state, state->level);

    bomber_app_start(state);
}

// Go into Level Select menu
void bomber_app_select_level(BomberAppState* state) {
    FURI_LOG_I(TAG, "Select Level");
    bomber_app_set_mode(state, BomberAppMode_LevelSelect);
}