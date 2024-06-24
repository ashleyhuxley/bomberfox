#include "bomber_ui.h"
#include "types.h"
#include "helpers.h"

// Renders the game to the viewport - called while playing
// canvas: Pointer to the canvas to draw to
// state: Pointer to the game state
static void render_game(Canvas* canvas, BomberAppState* state)
{
    // Draw bombs
    for (int i = 0; i < 10; i++)
    {
        Bomb bomb = state->bombs[i];
        switch(bomb.state)
        {
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

    // Draw
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            int ax = x * 8;
            int ay = y * 8;

            // Draw players
            if (x == state->fox.x && y == state->fox.y)
            {
                canvas_draw_xbm(canvas, ax, ay, 8, 8, fox_glyph);
            }
            if (x == state->wolf.x && y == state->wolf.y)
            {
                canvas_draw_xbm(canvas, ax, ay, 8, 8, wolf_glyph);
            }

            BlockType block = (BlockType)(state->level)[ix(x, y)];

            switch (block)
            {
                case BlockType_Empty:
                    break;
                case BlockType_Brick:
                    canvas_draw_xbm(canvas, ax, ay, 8, 8, brick_glyph);
                    break;
                default:
                    break;
            }
        }
    }
}

// Renders the menu to the viewport - called while in the menu
// canvas: Pointer to the canvas to draw to
// state: Pointer to the game state
static void render_menu(Canvas* canvas, BomberAppState* state)
{
    FURI_LOG_T(TAG, "render_menu");

    uint8_t ax = 20;
    if (state->isPlayerTwo)
    {
        ax = 70;
    }

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 29, 58, "Fox");
    canvas_draw_str(canvas, 79, 58, "Wolf");
    canvas_draw_xbm(canvas, 25, 21, 25, 25, fox);
    canvas_draw_xbm(canvas, 76, 21, 25, 25, wolf);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 15, 14, "Choose your player");
    canvas_draw_xbm(canvas, ax, 52, 4, 7, select_glyph);
}

// Main callback that starts off rendering
// canvas: Pointer to the canvas to draw to
// context: Pointer to the game state
static void bomber_ui_render_callback(Canvas* canvas, void* context)
{
    FURI_LOG_T(TAG, "bomber_ui_render_callback");

    furi_assert(context);
    BomberAppState* state = context;

    if(furi_mutex_acquire(state->data_mutex, 200) != FuriStatusOk)
    {
        return;
    }

    canvas_set_bitmap_mode(canvas, true);

    switch (state->mode)
    {
        case BomberAppMode_Playing:
            render_game(canvas, state);
            break;
        case BomberAppMode_Menu:
            render_menu(canvas, state);
        default:
            break;
    }
    
    // always release the mutex
    furi_mutex_release(state->data_mutex);
}

// Main callback that handles input and puts it on the queue
// input_event: The input event
// context_q: Pointer to the message queue
static void bomber_ui_input_callback(InputEvent* input_event, void* context_q)
{
    FURI_LOG_T(TAG, "bomber_ui_input_callback");
    FURI_LOG_I(TAG, "Input event received");
    furi_assert(context_q);
    FuriMessageQueue* queue = context_q;
    BomberEvent event = {.type = BomberEventType_Input, .input = *input_event};
    furi_message_queue_put(queue, &event, FuriWaitForever);
}

void bomber_ui_init(BomberAppState* state)
{
    FURI_LOG_T(TAG, "bomber_ui_init");

    state->view_port = view_port_alloc();
    view_port_draw_callback_set(state->view_port, bomber_ui_render_callback, state);
    view_port_input_callback_set(state->view_port, bomber_ui_input_callback, state->queue);
    state->gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);
}

void bomber_ui_destroy(BomberAppState* state)
{
    FURI_LOG_T(TAG, "bomber_ui_destroy");

    view_port_enabled_set(state->view_port, false);
    gui_remove_view_port(state->gui, state->view_port);
    view_port_free(state->view_port);
    furi_record_close(RECORD_GUI);
}