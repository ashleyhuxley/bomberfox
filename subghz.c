#include "subghz.h"
#include "bomber.h"

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