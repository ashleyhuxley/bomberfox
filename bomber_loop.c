#include "bomber.h"
#include "bomber_loop.h"

static void bomber_app_stop_playing(BomberAppState* state) 
{
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    // tag_ir_rx_stop();
    state->mode = BomberAppMode_Finished;
    furi_mutex_release(state->data_mutex);
}

static void bomber_app_quit(BomberAppState* state)
{
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    if(state->mode == BomberAppMode_Playing)
    {
        // tag_ir_rx_stop();
    }

    state->mode = BomberAppMode_Quit;
    furi_mutex_release(state->data_mutex);
}

static void bomber_app_error(BomberAppState* state)
{
    furi_mutex_acquire(state->data_mutex, FuriWaitForever);
    if(state->mode == BomberAppMode_Playing)
    {
        //tag_ir_rx_stop();
    }

    state->mode = BomberAppMode_Error;
    furi_mutex_release(state->data_mutex);
}

static bool bomber_app_handle_input(BomberAppState* state, InputEvent input)
{
    if(input.type == InputTypeShort && input.key == InputKeyOk)
    {

    }

    if(input.type == InputTypeShort && input.key == InputKeyBack)
    {
        if(state->mode == BomberAppMode_Playing)
        {
            bomber_app_stop_playing(state);
            return true;
        }
    }

    if(input.type == InputTypeLong && input.key == InputKeyBack)
    {
        bomber_app_quit(state);
        return false; // don't try to update the ui while quitting
    }

    return false;
}

void bomber_main_loop(BomberAppState* state)
{
    FURI_LOG_I(TAG, "bomber_main_loop");
    view_port_update(state->view_port);

    BomberEvent event;

    state->running = true;

    while(state->running)
    {

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
                    //case BomberEventType_InfraredMessage:
                    //    FURI_LOG_D(
                    //        TAG,
                    //        "Received ir with address: %lu, command: %lu",
                    //        event.ir_message->address,
                    //        event.ir_message->command);
                    //    state->data->last_ir_address = event.ir_message->address;
                    //    state->data->last_ir_command = event.ir_message->command;
                    //    updated = true;
                    //    break;
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