#ifndef BOMBER_HEADERS
#define BOMBER_HEADERS

#include <gui/gui.h>
#include <furi_hal.h>

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
    BomberAppMode mode;
    level* level;
    bool running;
    Player player;
} BomberAppState;

BomberAppState* bomber_app_state_get();

#endif