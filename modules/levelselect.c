#include "modules/levelselect.h"

// Whether or not to render a pixel in the level select menu
static bool is_solid_block(BlockType type) {
    return type == BlockType_Brick ||
           type == BlockType_PuBombStrength_Hidden ||
           type == BlockType_PuExtraBomb_Hidden ||
           type == BlockType_Wall;
}

bool bomber_levelselect_input(BomberAppState* state, InputEvent input) {
    if(input.type == InputTypeShort) {
        switch(input.key) {
            case InputKeyOk:
                bomber_app_setup_level(state);
                return true;
            case InputKeyUp:
                state->selectedLevel -= 2;
                break;
            case InputKeyDown:
                state->selectedLevel += 2;
                break;
            case InputKeyLeft:
                state->selectedLevel -= 1;
                break;
            case InputKeyRight:
                state->selectedLevel += 1;
                break;
            default:
                return false;
        }
    }

    uint8_t levelCount = sizeof(all_levels) / sizeof(int);
    if (state->selectedLevel >= levelCount) {
        state->selectedLevel = levelCount - 1;
    }

    return true;
}

// Render a small preview of a level for the level select menu
static void bomber_levelselect_render_preview(Canvas* canvas, uint8_t offset_x, uint8_t offset_y, uint8_t* level) {
    for(uint8_t x = 0; x < MAX_X; x++) {
        for(uint8_t y = 0; y < MAX_Y; y++) {
            BlockType block = (BlockType)level[ix(x, y)];
            if (is_solid_block(block)) {
                canvas_draw_frame(canvas, offset_x + (x * 2), offset_y + (y * 2), 2, 2);
            }
        }
    }
}

// Render the Level Select menu
void bomber_levelselect_render(Canvas* canvas, BomberAppState* state) {
    furi_assert(state);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 31, 12, "Select Level");

    uint8_t page_number = state->selectedLevel / ITEMS_PER_PAGE;
    uint8_t start = page_number * ITEMS_PER_PAGE;
    uint8_t levelCount = sizeof(all_levels) / sizeof(int);

    for (uint8_t ix = 0; ix < ITEMS_PER_PAGE; ix++) {
        uint8_t displayLevel = start + ix;

        if (displayLevel < levelCount) {
            uint8_t x = ix % 2 == 0 ? 18 : 75;
            uint8_t y = ix > 1 ? 39 : 15;

            canvas_draw_frame(canvas, x, y, 36, 20);
            bomber_levelselect_render_preview(canvas, x + 2, y + 2, all_levels[displayLevel]);
            if (displayLevel == state->selectedLevel) {
                canvas_draw_xbm(canvas, x - 6, y + 5, 4, 7, select_glyph);
            }
        }
    }
}