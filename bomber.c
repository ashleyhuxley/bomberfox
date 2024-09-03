#include "bomber.h"

static BomberAppState* state;

BomberAppState* bomber_app_state_get()
{
    return state;
}

bool bomber_app_init()
{
    FURI_LOG_T(TAG, "bomber_app_init");

    // Allocate application state
    state = malloc(sizeof(BomberAppState));
    if (!state) {
        FURI_LOG_E(TAG, "Failed to allocate memory for BomberAppState");
        return false;
    }

    // Allocate mutex
    state->data_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if (!state->data_mutex)
    {
        FURI_LOG_E(TAG, "Failed to allocate data mutex");
        return false;
    }

    state->timer_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if (!state->timer_mutex)
    {
        FURI_LOG_E(TAG, "Failed to allocate timer mutex");
        return false;
    }

    state->mode = BomberAppMode_Uninitialised;

    // Allocate message queue
    state->queue = furi_message_queue_alloc(8, sizeof(BomberEvent));
    if (!state->queue)
    {
        FURI_LOG_E(TAG, "Failed to allocate message queue");
        return false;
    }

    // Open notification record
    state->notification = furi_record_open(RECORD_NOTIFICATION);
    if (!state->notification)
    {
        FURI_LOG_E(TAG, "Failed to open notification record");
        return false;
    }
   
    // Allocate timer
    state->timer = furi_timer_alloc(bomber_callback_timer_tick, FuriTimerTypePeriodic, state->queue);
    if (!state->timer)
    {
        FURI_LOG_E(TAG, "Failed to allocate timer");
        return false;
    }

    // Allocate subghz worker
    state->subghz_worker = subghz_tx_rx_worker_alloc();
    if (!state->subghz_worker)
    {
        FURI_LOG_E(TAG, "Failed to allocate SubGhz worker");
        return false;
    }

    state->frequency = DEFAULT_FREQ;
    subghz_tx_rx_worker_set_callback_have_read(state->subghz_worker, bomber_callback_subghz_read, state);
    subghz_devices_init();
    state->subghz_device = radio_device_loader_set(state->subghz_device, SubGhzRadioDeviceTypeExternalCC1101);
    subghz_devices_reset(state->subghz_device);
    subghz_devices_idle(state->subghz_device);

    FURI_LOG_T(TAG, state->subghz_device->name);

    // Keep the backlight on
    notification_message_block(state->notification, &sequence_display_backlight_enforce_on);

    subghz_tx_rx_worker_start(state->subghz_worker, state->subghz_device, state->frequency);

    // Init UI
    state->view_port = view_port_alloc();
    if (!state->view_port)
    {
        FURI_LOG_E(TAG, "Failed to allocate viewport");
        return false;
    }

    view_port_draw_callback_set(state->view_port, bomber_callback_ui_render, state);
    view_port_input_callback_set(state->view_port, bomber_callback_ui_input, state->queue);
    state->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);

    return true;
}

void bomber_app_destroy()
{
    FURI_LOG_T(TAG, "Destroying app:");

    if (state->timer)
    {
        FURI_LOG_T(TAG, "  Destroying timer");
        furi_timer_free(state->timer);
        state->timer = NULL;
    }

    FURI_LOG_T(TAG, "  Removing notification");
    notification_message_block(state->notification, &sequence_display_backlight_enforce_auto);
    furi_record_close(RECORD_NOTIFICATION);

    FURI_LOG_T(TAG, "  Destroying Viewport & GUI");
    view_port_enabled_set(state->view_port, false);
    gui_remove_view_port(state->gui, state->view_port);
    view_port_free(state->view_port);
    state->view_port = NULL;
    furi_record_close(RECORD_GUI);
    
    if (subghz_tx_rx_worker_is_running(state->subghz_worker))
    {
        FURI_LOG_T(TAG, "  Stopping SubGhz worker");
        subghz_tx_rx_worker_stop(state->subghz_worker);
    }
    
    FURI_LOG_T(TAG, "  Freeing SubGhz device");
    radio_device_loader_end(state->subghz_device);
    subghz_devices_deinit();
    subghz_tx_rx_worker_free(state->subghz_worker);

    if (state->queue)
    {
        FURI_LOG_T(TAG, "  Freeing message queue");
        furi_message_queue_free(state->queue);
        state->queue = NULL;
    }

    if (state->data_mutex)
    {
        FURI_LOG_T(TAG, "  Freeing data mutex");
        furi_mutex_free(state->data_mutex);
        state->data_mutex = NULL;
    }

    if (state->timer_mutex)
    {
        FURI_LOG_T(TAG, "  Freeing timer mutex");
        furi_mutex_free(state->timer_mutex);
        state->timer_mutex = NULL;
    }
    
    if (state)
    {
        FURI_LOG_T(TAG, "  Freeing app state memory");
        free(state);
        state = NULL;
    }
}

// APPLICATION MAIN ENTRY POINT
int32_t bomber_main(void* p)
{
    UNUSED(p);

    FURI_LOG_I(TAG, "Initializing app");

    if (!bomber_app_init())
    {
        bomber_app_destroy();
        return 1;
    }

    state->selectedLevel = 0;
    state->rxMode = RxMode_Command;
    state->isDead = false;
    state->suicide = false;
    
    bomber_app_select_player(state);

    bomber_main_loop(state);

    FURI_LOG_I(TAG, "Destroying app");
    bomber_app_destroy();
    return 0;
}
