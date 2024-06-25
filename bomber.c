#include <furi.h>

#include "bomber.h"
#include "bomber_ui.h"
#include "bomber_loop.h"
#include "levels.h"
#include "helpers.h"
#include "radio_device_loader.h"
#include "types.h"

static BomberAppState* state;

BomberAppState* bomber_app_state_get()
{
    return state;
}

/* Callback for RX events from the Sub-GHz worker. Records the current ticks as
 * the time of the last reception. */
static void have_read_cb(void* context)
{
    furi_assert(context);
    BomberAppState* state = context;

    state->last_time_rx_data = furi_get_tick();
}

void bomber_app_init()
{
    FURI_LOG_T(TAG, "bomber_app_init");

    state = malloc(sizeof(BomberAppState));
    if (!state) {
        FURI_LOG_E(TAG, "Failed to allocate memory for BomberAppState");
        return;
    }
    state->mode = BomberAppMode_Uninitialised;

    state->queue = furi_message_queue_alloc(8, sizeof(BomberEvent));
    if (!state->queue) {
        FURI_LOG_E(TAG, "Failed to allocate message queue");
        free(state);
        return;
    }

    // SubGhz
    state->subghz_worker = subghz_tx_rx_worker_alloc();
    if (!state->subghz_worker) {
        FURI_LOG_E(TAG, "Failed to allocate SubGhz worker");
        furi_message_queue_free(state->queue);
        free(state);
        return;
    }
    state->frequency = DEFAULT_FREQ;
    subghz_tx_rx_worker_set_callback_have_read(state->subghz_worker, have_read_cb, state);
    subghz_devices_init();
    state->subghz_device = radio_device_loader_set(state->subghz_device, SubGhzRadioDeviceTypeExternalCC1101);
    subghz_devices_reset(state->subghz_device);
    subghz_devices_idle(state->subghz_device);

    state->notification = furi_record_open(RECORD_NOTIFICATION);
    if (!state->notification) {
        FURI_LOG_E(TAG, "Failed to open notification record");
        subghz_tx_rx_worker_free(state->subghz_worker);
        furi_message_queue_free(state->queue);
        free(state);
        return;
    }
    notification_message_block(state->notification, &sequence_display_backlight_enforce_on);

    state->timer = furi_timer_alloc(bomber_game_update_timer_callback, FuriTimerTypePeriodic, state->queue);
    if (!state->timer) {
        FURI_LOG_E(TAG, "Failed to allocate timer");
        furi_record_close(RECORD_NOTIFICATION);
        subghz_tx_rx_worker_free(state->subghz_worker);
        furi_message_queue_free(state->queue);
        free(state);
        return;
    }
    furi_timer_start(state->timer, furi_ms_to_ticks(50));

    state->bomb_ix = 0;
}

void bomber_game_update_timer_callback()
{
    FURI_LOG_T(TAG, "bomber_game_update_timer_callback");

    for (int i = 0; i < 10; i++)
    {
        // Update the bombs based on how long it's been since they were planted
        Bomb bomb = state->bombs[i];
        if (bomb.state != BombState_None)
        {
            uint32_t time = furi_get_tick() - bomb.planted;
            if (time > furi_ms_to_ticks(2000)) { state->bombs[i].state = BombState_Hot; }
            if (time > furi_ms_to_ticks(2100)) { state->bombs[i].state = BombState_Planted; }
            if (time > furi_ms_to_ticks(2200)) { state->bombs[i].state = BombState_Hot; }
            if (time > furi_ms_to_ticks(2300)) { state->bombs[i].state = BombState_Planted; }
            if (time > furi_ms_to_ticks(2400)) { state->bombs[i].state = BombState_Hot; }
            if (time > furi_ms_to_ticks(2500)) {
                state->bombs[i].state = BombState_Explode;
                (state->level)[ix(bomb.x - 1, bomb.y)] = BlockType_Empty;
                (state->level)[ix(bomb.x + 1, bomb.y)] = BlockType_Empty;
                (state->level)[ix(bomb.x, bomb.y - 1)] = BlockType_Empty;
                (state->level)[ix(bomb.x, bomb.y + 1)] = BlockType_Empty;
            }
            if (time > furi_ms_to_ticks(2600)) {
                state->bombs[i].planted = 0;
                state->bombs[i].state = BombState_None;
            }
        }
    }

    view_port_update(state->view_port);
}

void bomber_app_destroy()
{
    FURI_LOG_T(TAG, "bomber_app_destroy");
    furi_timer_free(state->timer);
    furi_message_queue_free(state->queue);
    
    notification_message_block(state->notification, &sequence_display_backlight_enforce_auto);

    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    furi_mutex_free(state->data_mutex);

    if (subghz_tx_rx_worker_is_running(state->subghz_worker)) {
        subghz_tx_rx_worker_stop(state->subghz_worker);
    }
    
    radio_device_loader_end(state->subghz_device);
    subghz_devices_deinit();
    subghz_tx_rx_worker_free(state->subghz_worker);
    
    free(state);
}

// APPLICATION MAIN ENTRY POINT
int32_t bomber_main(void* p)
{
    FURI_LOG_T(TAG, "bomber_app");
    UNUSED(p);

    FURI_LOG_I(TAG, "Initializing app");
    bomber_app_init();
    bomber_ui_init(state);

    bomber_app_set_mode(state, BomberAppMode_Menu);

    // TODO: This should be moved to after the menu
    state->level = level1;

    // Figure out player starting positions from level data
    state->fox = bomber_app_get_block(state->level, BlockType_Fox);
    state->level[ix(state->fox.x, state->fox.y)] = (uint8_t)BlockType_Empty;
    state->wolf = bomber_app_get_block(state->level, BlockType_Wolf);
    state->level[ix(state->wolf.x, state->wolf.y)] = (uint8_t)BlockType_Empty;

    state->data_mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    bomber_main_loop(state);

    FURI_LOG_I(TAG, "Destroying app");
    bomber_ui_destroy(state);
    bomber_app_destroy();
    return 0;
}
