#include "subghz.h"

void subghz_tx_level_data(BomberAppState * state, uint8_t * data) {
    subghz_tx_rx_worker_write(state -> subghz_worker, data, LEVEL_SIZE);
}

// Transmit player position to other flipper
// player: Pointer to the current player structure
// state: Pointer to the game state
void subghz_tx_new_position(BomberAppState * state, Player * player) {
    furi_assert(state);
    furi_assert(player);

    // First hex digit of 1st byte is character (0 = Fox, 1 = Wolf)
    uint8_t action = ACTION_MOVE;
    if (state -> isPlayerTwo) {
        action = action | PLAYER_TWO;
    }

    state -> tx_buffer[0] = action;
    state -> tx_buffer[1] = player -> x;
    state -> tx_buffer[2] = player -> y;

    // Transmit the buffer
    FURI_LOG_I(
        TAG, "Transmitting new position: action=0x%02X, x=%d, y=%d", action, player -> x, player -> y);
    subghz_tx_rx_worker_write(state -> subghz_worker, state -> tx_buffer, RX_TX_BUFFER_SIZE);
}

void subghz_tx_bomb_placement(BomberAppState * state, uint8_t x, uint8_t y) {
    uint8_t action = ACTION_BOMB;
    if (state -> isPlayerTwo) {
        action = action | PLAYER_TWO;
    }

    state -> tx_buffer[0] = action;
    state -> tx_buffer[1] = x;
    state -> tx_buffer[2] = y;

    // Transmit the bomb
    FURI_LOG_I(TAG, "Transmitting bomb: action=0x%02X, x=%d, y=%d", action, x, y);
    subghz_tx_rx_worker_write(state -> subghz_worker, state -> tx_buffer, RX_TX_BUFFER_SIZE);
}

void subghz_tx_death(BomberAppState * state) {
    uint8_t action = ACTION_DEATH;
    if (state -> isPlayerTwo) {
        action = action | PLAYER_TWO;
    }

    state -> tx_buffer[0] = action;
    if (state -> suicide) {
        state -> tx_buffer[1] = DEATH_SUICIDE;
    } else {
        state -> tx_buffer[1] = DEATH_MURDER;
    }
    state -> tx_buffer[2] = ACTION_NONE;

    FURI_LOG_I(TAG, "Transmitting death: action=0x%02X", action);
    subghz_tx_rx_worker_write(state -> subghz_worker, state -> tx_buffer, RX_TX_BUFFER_SIZE);
}

// Handle the buffer once data receive has completed
// state: Pointer to the application state
// rx_size: Number of bytes of data received
void bomber_game_post_rx(BomberAppState * state, size_t rx_size) {
    furi_assert(state);
    furi_assert(rx_size);

    if (rx_size == 0) {
        FURI_LOG_W(TAG, "Received data size is 0, ignoring");
        return;
    }

    furi_check(rx_size <= RX_TX_BUFFER_SIZE);
    FURI_LOG_T(TAG, "Received data size: %zu", rx_size);
    FURI_LOG_D(
        TAG,
        "Received data: 0x%02X 0x%12X 0x%22X",
        state -> rx_buffer[0],
        state -> rx_buffer[1],
        state -> rx_buffer[2]);

    Player * player = & state -> fox;
    if (state -> rx_buffer[0] >= PLAYER_TWO) {
        player = & state -> wolf;
        state -> rx_buffer[0] -= PLAYER_TWO;
    }

    switch (state -> rx_buffer[0]) {
    case ACTION_MOVE:
        bomber_game_player_move(state, player, state -> rx_buffer[1], state -> rx_buffer[2]);
        break;
    case ACTION_BOMB:
        FURI_LOG_T(TAG, "Hostile bomb at index %zu", player -> bomb_ix);

        player -> bombs[player -> bomb_ix].x = state -> rx_buffer[1];
        player -> bombs[player -> bomb_ix].y = state -> rx_buffer[2];

        player -> bombs[player -> bomb_ix].planted = furi_get_tick();
        player -> bombs[player -> bomb_ix].state = BombState_Planted;
        player -> bomb_ix = (player -> bomb_ix + 1) % 10;
        break;
    case ACTION_DEATH:
        if (state -> rx_buffer[1] == DEATH_SUICIDE) {
            state -> suicide = true;
        }
        bomber_app_gameover(state);
        dolphin_deed(DolphinDeedPluginGameWin);
        break;
    }
}

// Handle incoming subghz data
// state: Pointer to the application state
void subghz_check_incoming(BomberAppState * state) {
    FURI_LOG_T(TAG, "subghz_check_incoming");

    size_t avail = 0;
    while ((avail = subghz_tx_rx_worker_available(state -> subghz_worker)) > 0) {
        FURI_LOG_D(TAG, "Received data size: %zu", avail);

        uint32_t since_last_rx = furi_get_tick() - state -> last_time_rx_data;
        if (avail < RX_TX_BUFFER_SIZE && since_last_rx < MESSAGE_COMPLETION_TIMEOUT) {
            break;
        }

        size_t rx_size =
            subghz_tx_rx_worker_read(state -> subghz_worker, state -> rx_buffer, RX_TX_BUFFER_SIZE);

        BomberEvent event = {
            .type = BomberEventType_SubGhz,
            .subGhzIncomingSize = rx_size
        };

        if (furi_message_queue_put(state -> queue, & event, FuriWaitForever) != FuriStatusOk) {
            FURI_LOG_W(TAG, "Failed to put subghz event in message queue");
        }
    }
}

void subghz_check_incoming_leveldata(BomberAppState * state) {
    FURI_LOG_T(TAG, "subghz_check_incoming_leveldata");

    size_t avail = 0;
    while ((avail = subghz_tx_rx_worker_available(state -> subghz_worker)) > 0) {
        FURI_LOG_D(TAG, "Received level data size: %zu", avail);

        uint32_t since_last_rx = furi_get_tick() - state -> last_time_rx_data;
        if (avail < LEVEL_SIZE && since_last_rx < MESSAGE_COMPLETION_TIMEOUT) {
            break;
        }

        size_t rx_size =
            subghz_tx_rx_worker_read(state -> subghz_worker, state -> levelData, LEVEL_SIZE);
        furi_assert(rx_size);

        BomberEvent event = {
            .type = BomberEventType_HaveLevelData
        };

        if (furi_message_queue_put(state -> queue, & event, FuriWaitForever) != FuriStatusOk) {
            FURI_LOG_W(TAG, "Failed to put level data event in message queue");
        }
    }
}