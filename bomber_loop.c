#include "bomber_loop.h"

// Main entry point for handling input
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be updated, else false
static bool bomber_app_handle_input(BomberAppState * state, InputEvent input) {
    if (input.key == InputKeyBack) {
        bomber_app_quit(state);
        return false; // don't try to update the UI while quitting
    }

    switch (state -> mode) {
    case BomberAppMode_Playing:
        return bomber_game_input(state, input);
    case BomberAppMode_PlayerSelect:
        return bomber_playerselect_input(state, input);
    case BomberAppMode_LevelSelect:
        return bomber_levelselect_input(state, input);
    default:
        break;
    }

    return false;
}

// Application main loop
// state: Pointer to the application state
void bomber_main_loop(BomberAppState * state) {
    FURI_LOG_I(TAG, "Begin application main loop");
    view_port_update(state -> view_port);

    BomberEvent event;

    state -> running = true;

    while (state -> running) {
        switch (state -> rxMode) {
        case RxMode_Command:
            subghz_check_incoming(state);
            break;
        case RxMode_LevelData:
            subghz_check_incoming_leveldata(state);
            break;
        }
        bool updated = false;

        switch (furi_message_queue_get(state -> queue, & event, LOOP_MESSAGE_TIMEOUT_ms)) {
        case FuriStatusOk:

            switch (event.type) {
            case BomberEventType_Input:
                FURI_LOG_T(TAG, "Input Event from queue");
                updated = bomber_app_handle_input(state, event.input);
                break;
            case BomberEventType_Tick:
                FURI_LOG_T(TAG, "Tick Event from queue");
                updated = true;
                break;
            case BomberEventType_SubGhz:
                FURI_LOG_I(TAG, "SubGhz Event from queue");
                bomber_game_post_rx(state, event.subGhzIncomingSize);
                updated = true;
                break;
            case BomberEventType_HaveLevelData:
                FURI_LOG_I(TAG, "Level data event from queue");
                state -> level = state -> levelData;
                state -> rxMode = RxMode_Command;
                bomber_app_start(state);
                updated = true;
                break;
            default:
                FURI_LOG_E(TAG, "Unknown event received from queue.");
                break;
            }

            break;

            // error cases
        case FuriStatusErrorTimeout:
            FURI_LOG_T(TAG, "furi_message_queue_get timed out");
            break;
        default:
            FURI_LOG_E(TAG, "furi_message_queue_get was neither ok nor timeout");
            bomber_app_error(state);
        }

        updated |= bomber_game_tick(state);

        if (updated) {
            view_port_update(state -> view_port);
        }

        if (state -> mode == BomberAppMode_Quit || state -> mode == BomberAppMode_Error) {
            state -> running = false;
        }
    }
}