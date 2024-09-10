#ifndef STATE_H
#define STATE_H

#include <stdint.h>
#include <gui/gui.h>
#include <furi_hal.h>
#include "player.h"
#include <notification/notification_messages.h>
#include <lib/subghz/subghz_tx_rx_worker.h>

// Application mode. Rendering and input handlers rely on this to know what to render, or how to handle input.
typedef enum {
    BomberAppMode_Uninitialised,
    BomberAppMode_LevelSelect,
    BomberAppMode_PlayerSelect,
    BomberAppMode_Waiting,
    BomberAppMode_Ready,
    BomberAppMode_Playing,
    BomberAppMode_Finished,
    BomberAppMode_GameOver,
    BomberAppMode_Error,
    BomberAppMode_Quit
} BomberAppMode;

typedef enum {
    RxMode_Command,
    RxMode_LevelData
} RxMode;

typedef struct {
    FuriMessageQueue* queue; // Message queue
    FuriMutex* data_mutex; // Mutex
    FuriMutex* timer_mutex; // Mutex
    ViewPort* view_port; // Viewport
    Gui* gui; // GUI
    NotificationApp* notification;
    FuriTimer* timer;
    BomberAppMode mode;
    uint8_t* level; // Pointer to the current level array
    bool running;
    Player fox; // Position of the fox
    Player wolf; // Position of the wolf
    bool isPlayerTwo;
    uint8_t selectedLevel;

    bool isDead;
    bool suicide;

    // for Sub-GHz
    uint32_t frequency;
    SubGhzTxRxWorker* subghz_worker;
    const SubGhzDevice* subghz_device;
    volatile uint32_t last_time_rx_data;
    uint8_t rx_buffer[RX_TX_BUFFER_SIZE];
    uint8_t tx_buffer[RX_TX_BUFFER_SIZE];
    uint8_t levelData[LEVEL_SIZE];
    RxMode rxMode;
} BomberAppState;

#endif