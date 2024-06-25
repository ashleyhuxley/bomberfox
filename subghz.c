#include "subghz.h"
#include "types.h"

// Transmit player position to other flipper
// player: Pointer to the current player structure
// state: Pointer to the game state
void tx_new_position(Player* player, BomberAppState* state)
{
    // First hex digit of 1st byte is character (1 = Fox, 2 = Wolf)
    uint8_t action = 0x10;
    if(state->isPlayerTwo)
    {
        action += 0x10;
    }

    // Move action (Fox move = 0x11, Wolf move = 0x21)
    action += 0x01;

    state->tx_buffer[0] = action;
    state->tx_buffer[1] = player->x;
    state->tx_buffer[2] = player->y;

    // Transmit the buffer
    FURI_LOG_T(TAG, "Transmitting new position: action=0x%02X, x=%d, y=%d", action, player->x, player->y);
    subghz_tx_rx_worker_write(state->subghz_worker, state->tx_buffer, RX_TX_BUFFER_SIZE);
}

// Handle the buffer once data receive has completed
// state: Pointer to the application state
// rx_size: Number of bytes of data received
static void post_rx(BomberAppState* state, size_t rx_size)
{
    furi_assert(state);
    furi_assert(rx_size);

    if(rx_size == 0)
    {
        FURI_LOG_W(TAG, "Received data size is 0, ignoring");
        return;
    }

    // TODO: Handle incoming buffer

    // Ensure received size is within buffer limits
    furi_check(rx_size <= RX_TX_BUFFER_SIZE);
    FURI_LOG_T(TAG, "Received data size: %zu", rx_size);
}

// Handle incoming subghz data
// state: Pointer to the application state
void subghz_check_incoming(BomberAppState* state)
{
    size_t avail = 0;
    while((avail = subghz_tx_rx_worker_available(state->subghz_worker)) > 0)
    {
        uint32_t since_last_rx = furi_get_tick() - state->last_time_rx_data;
        if(avail < RX_TX_BUFFER_SIZE && since_last_rx < MESSAGE_COMPLETION_TIMEOUT)
        {
            break;
        }

        size_t rx_size = subghz_tx_rx_worker_read(state->subghz_worker, state->rx_buffer, RX_TX_BUFFER_SIZE);
        post_rx(state, rx_size);
    }
}