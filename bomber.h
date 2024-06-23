#ifndef BOMBER_HEADERS
#define BOMBER_HEADERS

#include <gui/gui.h>
#include <furi_hal.h>
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include <lib/subghz/subghz_tx_rx_worker.h>

#include "types.h"

#define TAG "bomb"

#define LOOP_MESSAGE_TIMEOUT_ms 500

#define DEFAULT_FREQ 433920000

#define RX_TX_BUFFER_SIZE 1024

typedef enum {
    BomberAppMode_Uninitialised,
    BomberAppMode_Menu,
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
    Player enemy;
    Bomb bombs[10];
    int bomb_ix;
    int now;
    bool isPlayerTwo;

    // for Sub-GHz
	uint32_t frequency;
	SubGhzTxRxWorker *subghz_worker;
	const SubGhzDevice *subghz_device;
    volatile uint32_t last_time_rx_data;
    uint8_t rx_buffer[RX_TX_BUFFER_SIZE];
	uint8_t tx_buffer[RX_TX_BUFFER_SIZE];
} BomberAppState;

BomberAppState* bomber_app_state_get();

void bomber_game_update_timer_callback();

#endif