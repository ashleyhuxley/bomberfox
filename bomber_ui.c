#include "bomber_ui.h"
#include "types.h"

static const uint8_t brick[] = {0xff,0x11,0xff,0x88,0xff,0x11,0xff,0x88};
//static const uint8_t bomb[] = {0x20,0x10,0x08,0x1e,0x3f,0x27,0x37,0x1e};

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

    // Draw
    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            //uint8_t value = (*state->level)[x][y];
            BlockType block = (BlockType)(*state->level)[y][x];

            int ax = x * 8;
            int ay = y * 8;

            switch (block)
            {
                case BlockType_Empty:
                    break;
                case BlockType_Brick:
                    canvas_draw_xbm(canvas, ax, ay, 8, 8, brick);
                    break;
                default:
                    break;
            }
        }
    }
    
    // always release the mutex
    furi_mutex_release(state->data_mutex);
}

void bomber_ui_init(BomberAppState* state)
{
    FURI_LOG_T(TAG, "bomber_ui_init");

    state->view_port = view_port_alloc();
    view_port_draw_callback_set(state->view_port, bomber_ui_render_callback, state);
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