#include "bomber.h"
#include "bomber_loop.h"
#include "helpers.h"
#include "subghz.h"

#define MESSAGE_COMPLETION_TIMEOUT 500

static void bomber_app_stop_playing(BomberAppState* state) 
{
    FURI_LOG_I(TAG, "Stop playing");

    furi_mutex_acquire(state->data_mutex, FuriWaitForever);

    state->mode = BomberAppMode_Finished;
    furi_mutex_release(state->data_mutex);
}

static void bomber_app_quit(BomberAppState* state)
{
    FURI_LOG_I(TAG, "Quitting");

    furi_mutex_acquire(state->data_mutex, FuriWaitForever);

    state->mode = BomberAppMode_Quit;
    furi_mutex_release(state->data_mutex);
}

static void bomber_app_error(BomberAppState* state)
{
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);

    state->mode = BomberAppMode_Error;
    furi_mutex_release(state->data_mutex);
}

// Handle direction keys to move the player around
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be update, else false
static bool bomber_app_handle_direction(BomberAppState* state, InputEvent input)
{
    Player* player = get_player(state);

    // Figure out where player will move to. If at an endge, don't move.
    Point newPoint = { player->x, player->y };

    if(input.key == InputKeyUp)
    {
        if (player->y == 0) { return false;}
        newPoint.y -= 1;       
    }

    if(input.key == InputKeyDown)
    {
        if (player->y >= 7) { return false; }
        newPoint.y += 1; 
    }

    if(input.key == InputKeyLeft)
    {
        if (player->x == 0) { return false; }
        newPoint.x -= 1; 
    }

    if(input.key == InputKeyRight)
    {
        if (player->x == 15) { return false; }
        newPoint.x += 1;
    }

    // Only allow move to new position if the block at that position is not occupied
    BlockType block = (BlockType)(state->level)[ix(newPoint.x, newPoint.y)];
    if (block == BlockType_Empty)
    {
        player->x = newPoint.x;
        player->y = newPoint.y;

        tx_new_position(player, state);

        return true;
    }

    return false;
}

// Handles input while on player select screen - just switch between Fox/Wolf
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be update, else false
static bool handle_menu_input(BomberAppState* state, InputEvent input)
{
    if (input.type == InputTypeShort && (input.key == InputKeyUp || input.key == InputKeyDown || input.key == InputKeyLeft || input.key == InputKeyRight))
    {
        state->isPlayerTwo = !state->isPlayerTwo;
        return true;
    }

    if (input.type == InputTypeShort && input.key == InputKeyOk)
    {
        state->mode = BomberAppMode_Playing;
        return true;
    }

    return false;
}

// Handle input while playing the game
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be update, else false
static bool handle_game_input(BomberAppState* state, InputEvent input)
{
    Player* player = get_player(state);

    if(input.type == InputTypeShort && input.key == InputKeyOk)
    {
        FURI_LOG_I(TAG, "Drop Bomb");
        Bomb bomb;
        bomb.x = player->x;
        bomb.y = player->y;
        bomb.state = BombState_Planted;
        bomb.planted = state->now;
        state->bombs[state->bomb_ix] = bomb;

        state->bomb_ix++;
        if (state->bomb_ix == 10)
        {
            state->bomb_ix = 0;
        }
    }

    if (input.type == InputTypeShort && (input.key == InputKeyUp || input.key == InputKeyDown || input.key == InputKeyLeft || input.key == InputKeyRight))
    {
        return bomber_app_handle_direction(state, input);
    }

    if(input.type == InputTypeShort && input.key == InputKeyBack)
    {
        if(state->mode == BomberAppMode_Playing)
        {
            bomber_app_stop_playing(state);
            return true;
        }
    }

    return false;
}

// Main entrypoint for handling input
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be update, else false
static bool bomber_app_handle_input(BomberAppState* state, InputEvent input)
{
    if(input.type == InputTypeLong && input.key == InputKeyBack)
    {
        bomber_app_quit(state);
        return false; // don't try to update the ui while quitting
    }

    switch (state->mode)
    {
        case BomberAppMode_Playing:
            return handle_game_input(state, input);
        case BomberAppMode_Menu:
            return handle_menu_input(state, input);
        default:
            break;
    }

    return false;
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
static void subghz_check_incoming(BomberAppState* state)
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

// Application main loop. 
// state: Pointer to the application state
void bomber_main_loop(BomberAppState* state)
{
    FURI_LOG_I(TAG, "bomber_main_loop");
    view_port_update(state->view_port);

    BomberEvent event;

    state->running = true;

    while(state->running)
    {
        subghz_check_incoming(state);

        switch(furi_message_queue_get(state->queue, &event, LOOP_MESSAGE_TIMEOUT_ms))
        {
            case FuriStatusOk:
                FURI_LOG_I(TAG, "Event from queue: %d", event.type);
                bool updated = false;
                switch(event.type)
                {
                    case BomberEventType_Input:
                        updated = bomber_app_handle_input(state, event.input);
                        break;
                    default:
                        FURI_LOG_E(TAG, "Unknown event received from queue.");
                        break;
                }

                if(updated)
                {
                    view_port_update(state->view_port);
                }
                break;

            // error cases
            case FuriStatusErrorTimeout:
                FURI_LOG_D(TAG, "furi_message_queue_get timed out");
                break;
            default:
                FURI_LOG_E(TAG, "furi_message_queue_get was neither ok nor timeout");
                bomber_app_error(state);
        }

        if(state->mode == BomberAppMode_Quit || state->mode == BomberAppMode_Error)
        {
            state->running = false;
        }
    }
}