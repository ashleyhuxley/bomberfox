#include "bomber_ui.h"
#include "types.h"
#include "helpers.h"
#include "levels.h"

// Draws a single bomb based on its state
static void draw_bomb(Canvas* canvas, Bomb bomb) {
    switch(bomb.state) {
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
static void draw_player(Canvas* canvas, int x, int y, const uint8_t* glyph) {
    canvas_draw_xbm(canvas, x * 8, y * 8, 8, 8, glyph);
}

// Draws a single block
static void draw_block(Canvas* canvas, int x, int y, BlockType block) {
    switch(block) {
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
    case BlockType_Empty:
    default:
        break;
    }
}

// Renders the game to the viewport - called while playing
static void render_game(Canvas* canvas, BomberAppState* state) {
    // Draw bombs
    for(uint8_t i = 0; i < MAX_BOMBS; i++) {
        draw_bomb(canvas, state->fox.bombs[i]);
        draw_bomb(canvas, state->wolf.bombs[i]);
    }

    // Draw players and blocks
    for(uint8_t x = 0; x < 16; x++) {
        for(uint8_t y = 0; y < 8; y++) {
            if(x == state->fox.x && y == state->fox.y) {
                draw_player(canvas, x, y, fox_glyph);
            }
            if(x == state->wolf.x && y == state->wolf.y) {
                draw_player(canvas, x, y, wolf_glyph);
            }

            BlockType block = (BlockType)(state->level)[ix(x, y)];
            draw_block(canvas, x, y, block);
        }
    }
}

// Renders the player select menu to the viewport - called while in the player select menu
static void render_player_select(Canvas* canvas, BomberAppState* state) {
    uint8_t ax = state->isPlayerTwo ? 70 : 20;

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 29, 58, "Fox");
    canvas_draw_str(canvas, 79, 58, "Wolf");
    canvas_draw_xbm(canvas, 25, 21, 25, 25, fox);
    canvas_draw_xbm(canvas, 76, 21, 25, 25, wolf);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 15, 14, "Choose your player");
    canvas_draw_xbm(canvas, ax, 52, 4, 7, select_glyph);
}

static void render_level_preview(Canvas* canvas, uint8_t offset_x, uint8_t offset_y, uint8_t* level) {
    for(uint8_t x = 0; x < 16; x++) {
        for(uint8_t y = 0; y < 8; y++) {
            BlockType block = (BlockType)level[ix(x, y)];
            if (is_solid_block(block)) {
                canvas_draw_frame(canvas, offset_x + (x * 2), offset_y + (y * 2), 2, 2);
            }
        }
    }
}

// Render the Level Select menu
static void render_level_select(Canvas* canvas, BomberAppState* state) {
    furi_assert(state);

    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str(canvas, 31, 12, "Select Level");
    canvas_draw_str(canvas, 9, 29, "1");
    canvas_draw_str(canvas, 9, 53, "3");
    canvas_draw_str(canvas, 66, 29, "2");
    canvas_draw_str(canvas, 66, 53, "4");

    canvas_draw_frame(canvas, 18, 15, 36, 20);
    canvas_draw_frame(canvas, 75, 15, 36, 20);
    canvas_draw_frame(canvas, 18, 39, 36, 20);
    canvas_draw_frame(canvas, 75, 39, 36, 20);

    render_level_preview(canvas, 20, 17, level1);
    render_level_preview(canvas, 77, 17, level2);
    render_level_preview(canvas, 20, 41, level3);
    render_level_preview(canvas, 77, 41, level4);

    switch (state->selectedLevel) {
        case 0:
            canvas_draw_frame(canvas, 7, 19, 10, 12);
            break;
        case 1:
            canvas_draw_frame(canvas, 64, 19, 10, 12);
            break;
        case 2:
            canvas_draw_frame(canvas, 7, 43, 10, 12);
            break;
        case 3:
            canvas_draw_frame(canvas, 64, 43, 10, 12);
            break;
    }
}

static void render_game_over(Canvas* canvas, BomberAppState* state) {
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 15, 14, "Game Over!");
    if(state->dead == WhoDied_Fox) {
        canvas_draw_str(canvas, 15, 30, "Wolf Wins!");
    } else {
        canvas_draw_str(canvas, 15, 30, "Fox Wins!");
    }
}

// Main callback that starts off rendering
void bomber_ui_render_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    BomberAppState* state = context;

    if(furi_mutex_acquire(state->data_mutex, 200) != FuriStatusOk) {
        FURI_LOG_W(TAG, "Failed to acquire mutex in render callback");
        return;
    }

    canvas_set_bitmap_mode(canvas, true);

    switch(state->mode) {
    case BomberAppMode_Playing:
        render_game(canvas, state);
        break;
    case BomberAppMode_PlayerSelect:
        render_player_select(canvas, state);
        break;
    case BomberAppMode_LevelSelect:
        render_level_select(canvas, state);
        break;
    case BomberAppMode_GameOver:
        render_game_over(canvas, state);
        break;
    default:
        break;
    }

    furi_mutex_release(state->data_mutex);
}

// Main callback that handles input and puts it on the queue
void bomber_ui_input_callback(InputEvent* input_event, void* context_q) {
    FURI_LOG_T(TAG, "bomber_ui_input_callback");
    furi_assert(context_q);

    FuriMessageQueue* queue = context_q;
    BomberEvent event = {.type = BomberEventType_Input, .input = *input_event};

    if(furi_message_queue_put(queue, &event, FuriWaitForever) != FuriStatusOk) {
        FURI_LOG_W(TAG, "Failed to put input event in message queue");
    }
}
