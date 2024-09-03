#ifndef SUBGHZ_H
#define SUBGHZ_H

#include "types/state.h"
#include "types/event.h"
#include "helpers.h"
#include "modules/modeselect.h"
#include "modules/gameplay.h"

#define MESSAGE_COMPLETION_TIMEOUT 500

#define ACTION_NONE 0x00
#define ACTION_MOVE 0x01
#define ACTION_BOMB 0x02
#define ACTION_DEATH 0x03

#define DEATH_MURDER 0x00
#define DEATH_SUICIDE 0x01

#define PLAYER_TWO 0x10

void subghz_tx_new_position(BomberAppState * state, Player * player);
void subghz_tx_bomb_placement(BomberAppState * state, uint8_t x, uint8_t y);
void subghz_tx_death(BomberAppState * state);
void subghz_tx_level_data(BomberAppState * state, uint8_t * data);

void subghz_check_incoming(BomberAppState * state);
void subghz_check_incoming_leveldata(BomberAppState * state);
void bomber_game_post_rx(BomberAppState * state, size_t rx_size);

#endif