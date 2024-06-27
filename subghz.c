#include "subghz.h"
#include "types.h"

// Transmit player position to other flipper
// player: Pointer to the current player structure
// state: Pointer to the game state
void tx_new_position(Player* player, BomberAppState* state)
{
    furi_assert(state);
    furi_assert(player);

    // First hex digit of 1st byte is character (0 = Fox, 1 = Wolf)
    uint8_t action = 0x00;
    if(state->isPlayerTwo)
    {
        action = action | 0x10;
    }

    // Move action (Fox move = 0x01, Wolf move = 0x11)
    action += 0x01;

    state->tx_buffer[0] = action;
    state->tx_buffer[1] = player->x;
    state->tx_buffer[2] = player->y;

    // Transmit the buffer
    FURI_LOG_I(TAG, "Transmitting new position: action=0x%02X, x=%d, y=%d", action, player->x, player->y);
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

    // Ensure received size is within buffer limits
    furi_check(rx_size <= RX_TX_BUFFER_SIZE);
    FURI_LOG_T(TAG, "Received data size: %zu", rx_size);
    FURI_LOG_T(TAG, "Received data: 0x%02X 0x%12X 0x%22X", state->rx_buffer[0], state->rx_buffer[1], state->rx_buffer[2]);

    // TODO: Handle other actions
    Player* player = (state->rx_buffer[0] > 0x10) ? &state->wolf : &state->fox;
    player->x = state->rx_buffer[1];
    player->y = state->rx_buffer[2];
    view_port_update(state->view_port);
}

// Handle incoming subghz data
// state: Pointer to the application state
void subghz_check_incoming(BomberAppState* state)
{
    FURI_LOG_T(TAG, "subghz_check_incoming");

    size_t avail = 0;
    while((avail = subghz_tx_rx_worker_available(state->subghz_worker)) > 0)
    {
        FURI_LOG_D(TAG, "Received data size: %zu", avail);

        uint32_t since_last_rx = furi_get_tick() - state->last_time_rx_data;
        if(avail < RX_TX_BUFFER_SIZE && since_last_rx < MESSAGE_COMPLETION_TIMEOUT)
        {
            break;
        }

        size_t rx_size = subghz_tx_rx_worker_read(state->subghz_worker, state->rx_buffer, RX_TX_BUFFER_SIZE);
        post_rx(state, rx_size);
    }
}