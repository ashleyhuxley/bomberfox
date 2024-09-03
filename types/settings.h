#ifndef SETTINGS_H
#define SETTINGS_H

#define TAG "bomb"
#define LOOP_MESSAGE_TIMEOUT_ms 50
#define DEFAULT_FREQ 433920000
#define RX_TX_BUFFER_SIZE 3
#define MAX_BOMBS 10
#define LEVEL_SIZE 128
#define POWERUP_EXTRABOMB_RATIO 0.1
#define POWERUP_BOMBPOWER_RATIO 0.15
#define BOMB_HOT_TIME furi_ms_to_ticks(2000)
#define BOMB_PLANTED_TIME furi_ms_to_ticks(2100)
#define BOMB_EXPLODE_TIME furi_ms_to_ticks(2500)
#define BOMB_RESET_TIME furi_ms_to_ticks(2600)
#define MAX_X 16
#define MAX_Y 8

#endif