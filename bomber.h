#ifndef BOMBER_HEADERS
#define BOMBER_HEADERS

#include <gui/gui.h>
#include <furi_hal.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>

#include "types.h"

#define TAG "bomb"

#define LOOP_MESSAGE_TIMEOUT_ms 500

typedef enum {
    BomberAppMode_Uninitialised,
    BomberAppMode_Ready,
    BomberAppMode_Playing,
    BomberAppMode_Finished,
    BomberAppMode_Error,
    BomberAppMode_Quit
} BomberAppMode;

typedef enum {
    BomberEventType_Input,
    BomberEventType_InfraredMessage,
    BomberEventType_Tick,
} BomberEventType;

typedef struct {
    BomberEventType type;
    //InfraredMessage* ir_message;
    InputEvent input;
} BomberEvent;

typedef struct {
    FuriMessageQueue* queue;
    FuriMutex* data_mutex;
    ViewPort* view_port;
    Gui* gui;
    NotificationApp* notification;
    FuriTimer* timer;
    BomberAppMode mode;
    uint8_t* level;
    bool running;
    Player player;
    Bomb bombs[10];
    int bomb_ix;
    int now;
} BomberAppState;

BomberAppState* bomber_app_state_get();

void bomber_game_update_timer_callback();

#endif