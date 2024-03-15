#include <furi.h>
#include "bomber.h"
#include "bomber_ui.h"
#include "bomber_loop.h"
#include "levels.h"
#include "helpers.h"

static BomberAppState* state;

BomberAppState* bomber_app_state_get() {
    return state;
}

void bomber_app_init() {
    FURI_LOG_T(TAG, "bomber_app_init");
    state = malloc(sizeof(BomberAppState));
    state->queue = furi_message_queue_alloc(8, sizeof(BomberEvent));
    state->level = &level1;
    state->player = bomber_app_get_player(state->level);
    state->data_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    state->mode = BomberAppMode_Uninitialised;
}

void bomber_app_destroy() {
    FURI_LOG_T(TAG, "bomber_app_destroy");
    furi_message_queue_free(state->queue);
    furi_mutex_free(state->data_mutex);
    free(state);
}

int32_t bomber_main(void* p) {
    FURI_LOG_T(TAG, "bomber_app");
    UNUSED(p);

    FURI_LOG_I(TAG, "Initialising app");
    bomber_app_init();
    bomber_ui_init(state);
    state->mode = BomberAppMode_Ready;

    bomber_main_loop(state);

    FURI_LOG_I(TAG, "Destroying app");
    bomber_ui_destroy(state);
    bomber_app_destroy();
    return 0;
}
