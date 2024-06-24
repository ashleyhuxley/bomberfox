#include "subghz.h"
#include "types.h"

// Transmit player position to other flipper
// player: Pointer to the current player structure
// state: Pointer to the game state
void tx_new_position(Player* player, BomberAppState* state)
{
    // First hex digit of 1st byte is character (1 = Fox, 2 = Wolf)
    uint8_t action = 0x10;
    if (state->isPlayerTwo)
    {
        action += 0x10;
    }

    // Move action (Fox move = 0x11, Wolf move = 0x21)
    action += 0x01;

    state->tx_buffer[0] = action;
    state->tx_buffer[1] = player->x;
    state->tx_buffer[2] = player->y;

    subghz_tx_rx_worker_write(state->subghz_worker, state->tx_buffer, RX_TX_BUFFER_SIZE);
}


// Handle the buffer once data receive has completed
// state: Pointer to the application state
// rx_size: Number of bytes of data recieved
static void post_rx(BomberAppState* state, size_t rx_size)
{
    furi_assert(state);
    furi_assert(rx_size);

    if (rx_size == 0) {
		return;
	}

    // TODO: Handle incoming buffer

    furi_check(rx_size <= RX_TX_BUFFER_SIZE);
}

// Handle incoming subghz data
// state: Pointer to the application state
void subghz_check_incoming(BomberAppState* state)
{
	/* if the maximum message size was reached or the
	 * MESSAGE_COMPLETION_TIMEOUT has expired, retrieve a message and call
	 * post_rx() */
	size_t avail = 0;
	while ((avail = subghz_tx_rx_worker_available(state->subghz_worker)) > 0)
    {
		volatile uint32_t since_last_rx = furi_get_tick() - state->last_time_rx_data;
		if (avail < RX_TX_BUFFER_SIZE && since_last_rx < MESSAGE_COMPLETION_TIMEOUT)
        {
			break;
		}

		size_t rx_size = subghz_tx_rx_worker_read(state->subghz_worker, state->rx_buffer, RX_TX_BUFFER_SIZE);
		post_rx(state, rx_size);
	}
}