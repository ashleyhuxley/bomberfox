#include "modules/gameplay.h"

void bomber_game_player_move(BomberAppState* state, Player* player, uint8_t x, uint8_t y) {
    player -> x = x;
    player -> y = y;

    BlockType block = (BlockType)(state -> level)[ix(player -> x, player -> y)];
    if (block == BlockType_PuBombStrength) {
        player -> bomb_power++;
        state -> level[ix(player -> x, player -> y)] = BlockType_Empty;
    } else if (block == BlockType_PuExtraBomb) {
        player -> bomb_count++;
        if (player -> bomb_count == MAX_BOMBS) {
            player -> bomb_count = MAX_BOMBS;
        }
        state -> level[ix(player -> x, player -> y)] = BlockType_Empty;
    }
}

// Draws a single bomb based on its state
static void bomber_game_draw_bomb(Canvas * canvas, Bomb bomb) {
    switch (bomb.state) {
    case BombState_Planted:
        canvas_draw_xbm(canvas, bomb.x * 8, bomb.y * 8, 8, 8, bomb_glyph);
        break;
    case BombState_Hot:
        canvas_draw_xbm(canvas, bomb.x * 8, bomb.y * 8, 8, 8, bomb_flash);
        break;
    case BombState_Explode:
        canvas_draw_xbm(canvas, bomb.x * 8, bomb.y * 8, 8, 8, bomb_explode);
        break;
    default:
        break;
    }
}

// Draws a single player
static void bomber_game_draw_player(Canvas * canvas, int x, int y,
    const uint8_t * glyph) {
    canvas_draw_xbm(canvas, x * 8, y * 8, 8, 8, glyph);
}

// Draws a single block
static void bomber_game_draw_block(Canvas * canvas, int x, int y, BlockType block) {
    switch (block) {
    case BlockType_Brick:
    case BlockType_PuBombStrength_Hidden:
    case BlockType_PuExtraBomb_Hidden:
        canvas_draw_xbm(canvas, x * 8, y * 8, 8, 8, brick_glyph);
        break;
    case BlockType_PuBombStrength:
        canvas_draw_xbm(canvas, x * 8, y * 8, 8, 8, powerup_bombpower_glyph);
        break;
    case BlockType_PuExtraBomb:
        canvas_draw_xbm(canvas, x * 8, y * 8, 8, 8, powerup_extrabomb_glyph);
        break;
    case BlockType_Wall:
        canvas_draw_xbm(canvas, x * 8, y * 8, 8, 8, wall_glyph);
        break;
    case BlockType_Empty:
    default:
        break;
    }
}

// Renders the game to the viewport - called while playing
void bomber_game_render(Canvas * canvas, BomberAppState * state) {
    // Draw bombs
    for (uint8_t i = 0; i < MAX_BOMBS; i++) {
        bomber_game_draw_bomb(canvas, state -> fox.bombs[i]);
        bomber_game_draw_bomb(canvas, state -> wolf.bombs[i]);
    }

    // Draw players and blocks
    for (uint8_t x = 0; x < MAX_X; x++) {
        for (uint8_t y = 0; y < MAX_Y; y++) {
            if (x == state -> fox.x && y == state -> fox.y) {
                bomber_game_draw_player(canvas, x, y, fox_glyph);
            }
            if (x == state -> wolf.x && y == state -> wolf.y) {
                bomber_game_draw_player(canvas, x, y, wolf_glyph);
            }

            BlockType block = (BlockType)(state -> level)[ix(x, y)];
            bomber_game_draw_block(canvas, x, y, block);
        }
    }
}

// Check if a particular coordingate is occupied by a players active bomb
static bool is_occupied_by_bomb(Player * player, uint8_t x, uint8_t y) {
    for (int i = 0; i < MAX_BOMBS; i++) {
        Bomb bomb = player -> bombs[i];
        if (bomb.state != BombState_None && bomb.x == x && bomb.y == y) {
            return true;
        }
    }

    return false;
}

// Handle direction keys to move the player around
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be updated, else false
static bool bomber_game_handle_direction(BomberAppState * state, InputEvent input) {
    Player * player = get_current_player(state);

    Point newPoint = {
        player -> x,
        player -> y
    };

    switch (input.key) {
    case InputKeyUp:
        if (player -> y == 0) return false;
        newPoint.y -= 1;
        break;
    case InputKeyDown:
        if (player -> y >= 7) return false;
        newPoint.y += 1;
        break;
    case InputKeyLeft:
        if (player -> x == 0) return false;
        newPoint.x -= 1;
        break;
    case InputKeyRight:
        if (player -> x >= 15) return false;
        newPoint.x += 1;
        break;
    default:
        return false;
    }

    // Only allow move to new position if the block at that position is not occupied
    BlockType block = (BlockType)(state -> level)[ix(newPoint.x, newPoint.y)];
    if (block == BlockType_Brick || block == BlockType_PuBombStrength_Hidden || block == BlockType_PuExtraBomb_Hidden || block == BlockType_Wall) {
        return false;
    }

    if (is_occupied_by_bomb( & state -> fox, newPoint.x, newPoint.y) ||
        is_occupied_by_bomb( & state -> wolf, newPoint.x, newPoint.y)) {
        return false;
    }

    bomber_game_player_move(state, player, newPoint.x, newPoint.y);

    subghz_tx_new_position(state, player);

    return true;
}

// Handle input while playing the game
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be updated, else false
bool bomber_game_input(BomberAppState * state, InputEvent input) {
    Player * player = get_current_player(state);

    if (input.type == InputTypeShort) {
        switch (input.key) {
        case InputKeyOk:
            FURI_LOG_I(TAG, "Drop Bomb");

            for (int i = 0; i < player -> bomb_count; i++) {
                if (player -> bombs[i].state == BombState_None) {
                    Bomb bomb;
                    bomb.x = player -> x;
                    bomb.y = player -> y;
                    bomb.state = BombState_Planted;
                    bomb.planted = furi_get_tick();

                    player -> bombs[i] = bomb;

                    subghz_tx_bomb_placement(state, bomb.x, bomb.y);
                    break;
                }
            }

            return true;
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyLeft:
        case InputKeyRight:
            return bomber_game_handle_direction(state, input);
        default:
            break;
        }
    }

    return false;
}

static bool bomber_game_handle_explosion(BomberAppState * state, uint8_t x, uint8_t y, bool ownBombs) {
    // Out of bounds.
    // No need to check negatives as uint8_t is unsigned and will underflow, resulting in a value way over MAX_X and MAX_Y.
    if (x >= MAX_X || y >= MAX_Y) {
        return false;
    }

    Player * player = get_current_player(state);

    if (player -> x == x && player -> y == y) {
        subghz_tx_death(state);
        state -> isDead = true;
        state -> suicide = ownBombs;
        bomber_app_gameover(state);
    }

    switch (state -> level[ix(x, y)]) {
    case BlockType_Brick:
        state -> level[ix(x, y)] = BlockType_Empty;
        return true;
    case BlockType_PuBombStrength_Hidden:
        state -> level[ix(x, y)] = BlockType_PuBombStrength;
        return true;
    case BlockType_PuExtraBomb_Hidden:
        state -> level[ix(x, y)] = BlockType_PuExtraBomb;
        return true;
    case BlockType_Wall:
        return true;
    default:
        return false;
    }
}

static bool bomber_game_update_bombs(Player * player, BomberAppState * state, bool ownBombs) {
    bool changed = false;

    for (uint8_t i = 0; i < MAX_BOMBS; i++) {
        Bomb * bomb = & player -> bombs[i];
        if (bomb -> state != BombState_None) {
            uint32_t time = furi_get_tick() - bomb -> planted;

            if (time > BOMB_RESET_TIME) {
                bomb -> planted = 0;
                bomb -> state = BombState_None;
                continue;
            }

            if (time > BOMB_EXPLODE_TIME) {

                bomb -> state = BombState_Explode;

                // Loop for X Negative direction
                for (uint8_t j = 0; j < player -> bomb_power + 1; j++) {
                    if (bomber_game_handle_explosion(state, bomb -> x - j, bomb -> y, ownBombs)) {
                        changed = true;
                        break;
                    }

                }

                // Loop for X Positive direction
                for (uint8_t k = 0; k < player -> bomb_power + 1; k++) {
                    if (bomber_game_handle_explosion(state, bomb -> x + k, bomb -> y, ownBombs)) {
                        changed = true;
                        break;
                    }
                }

                // Loop for Y Positive direction
                for (uint8_t l = 0; l < player -> bomb_power + 1; l++) {
                    if (bomber_game_handle_explosion(state, bomb -> x, bomb -> y + l, ownBombs)) {
                        changed = true;
                        break;
                    }
                }

                // Loop for Y Negative direction
                for (uint8_t m = 0; m < player -> bomb_power + 1; m++) {
                    if (bomber_game_handle_explosion(state, bomb -> x, bomb -> y - m, ownBombs)) {
                        changed = true;
                        break;
                    }
                }

                continue;
            }

            if (time > BOMB_PLANTED_TIME) {
                bomb -> state = BombState_Planted;
            } else if (time > BOMB_HOT_TIME) {
                bomb -> state = BombState_Hot;
            }
        }
    }
    return changed;
}

bool bomber_game_tick(BomberAppState * state) {
    bool changed = false;
    if (furi_mutex_acquire(state -> timer_mutex, 0) == FuriStatusOk) {
        changed &= bomber_game_update_bombs( & state -> fox, state, !state -> isPlayerTwo);
        changed &= bomber_game_update_bombs( & state -> wolf, state, state -> isPlayerTwo);
        furi_mutex_release(state -> timer_mutex);
    }
    return changed;
}