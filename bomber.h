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

#define RX_TX_BUFFER_SIZE 3

// Application mode. Rendering and input handlers rely on this to know what to render, or how to handle input.
typedef enum {
    BomberAppMode_Uninitialised,
    BomberAppMode_Menu,
    BomberAppMode_Ready,
    BomberAppMode_Playing,
    BomberAppMode_Finished,
    BomberAppMode_Error,
    BomberAppMode_Quit
} BomberAppMode;

// Event types for message queue
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
    FuriMessageQueue* queue;            // Message queue
    FuriMutex* data_mutex;              // Mutex
    ViewPort* view_port;                // Viewport
    Gui* gui;                           // GUI
    NotificationApp* notification;      
    FuriTimer* timer;
    BomberAppMode mode;
    uint8_t* level;                     // Pointer to the current level array
    bool running;                       
    Player fox;                         // Position of the fox
    Player wolf;                        // Position of the wolf
    Bomb bombs[10];                     // Array of bombs
    int bomb_ix;                        // Index of current bomb
    int now;                            // Basic tick timer (should probably use built in ticks)
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