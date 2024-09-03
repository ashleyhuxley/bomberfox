#include "modules/waitforplayer.h"

void bomber_wait_render(Canvas* canvas, BomberAppState* state) {
    furi_assert(state);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 15, 14, "Waiting for fox...");
}