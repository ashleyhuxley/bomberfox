#include "callbacks.h"

void bomber_callback_timer_tick()
{
    FURI_LOG_T(TAG, "Timer Callback");

    BomberAppState* state = bomber_app_state_get();

    if (bomber_game_tick(state))
    {
        BomberEvent event = {.type = BomberEventType_Tick };

        if(furi_message_queue_put(state->queue, &event, FuriWaitForever) != FuriStatusOk) {
            FURI_LOG_W(TAG, "Failed to put timer event in message queue");
        }
    }
}

/* Callback for RX events from the Sub-GHz worker. Records the current ticks as
 * the time of the last reception. */
void bomber_callback_subghz_read(void* context) {
    furi_assert(context);
    BomberAppState* state = context;

    state->last_time_rx_data = furi_get_tick();
}

// Main callback that starts off rendering
void bomber_callback_ui_render(Canvas* canvas, void* context) {
    furi_assert(context);
    BomberAppState* state = context;

    if(furi_mutex_acquire(state->data_mutex, 200) != FuriStatusOk) {
        FURI_LOG_W(TAG, "Failed to acquire mutex in render callback");
        return;
    }

    canvas_set_bitmap_mode(canvas, true);

    switch(state->mode) {
    case BomberAppMode_Playing:
        bomber_game_render(canvas, state);
        break;
    case BomberAppMode_PlayerSelect:
        bomber_playerselect_render(canvas, state);
        break;
    case BomberAppMode_LevelSelect:
        bomber_levelselect_render(canvas, state);
        break;
    case BomberAppMode_GameOver:
        bomber_gameover_render(canvas, state);
        break;
    case BomberAppMode_Waiting:
        bomber_wait_render(canvas, state);
    default:
        break;
    }

    furi_mutex_release(state->data_mutex);
}

// Main callback that handles input and puts it on the queue
void bomber_callback_ui_input(InputEvent* input_event, void* context_q) {
    FURI_LOG_T(TAG, "bomber_callback_ui_input");
    furi_assert(context_q);

    FuriMessageQueue* queue = context_q;
    BomberEvent event = {.type = BomberEventType_Input, .input = *input_event};

    if(furi_message_queue_put(queue, &event, FuriWaitForever) != FuriStatusOk) {
        FURI_LOG_W(TAG, "Failed to put input event in message queue");
    }
}