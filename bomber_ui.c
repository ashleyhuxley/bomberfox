#include "bomber_ui.h"
#include "types.h"
#include "helpers.h"

static const uint8_t brick_glyph[] = {0xff,0x11,0xff,0x88,0xff,0x11,0xff,0x88};
static const uint8_t player_glyph[] = {0x81,0xc3,0xbd,0x81,0x99,0x42,0x24,0x18};
static const uint8_t bomb_glyph[] = {0x20,0x10,0x08,0x1e,0x3f,0x27,0x37,0x1e};
static const uint8_t bomb_flash[] = {0x20,0x10,0x08,0x1e,0x21,0x21,0x21,0x1e};
static const uint8_t bomb_explode[] = {0x30,0x4b,0x8d,0x61,0x22,0x91,0xaa,0xcc};

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

            // Draw player
            if (x == state->player.x && y == state->player.y)
            {
                canvas_draw_xbm(canvas, ax, ay, 8, 8, player_glyph);
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
    
    // always release the mutex
    furi_mutex_release(state->data_mutex);
}

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