#include "modules/gameover.h"

void bomber_gameover_render(Canvas* canvas, BomberAppState* state) {
    furi_assert(state);
    if(state->isDead) {
        canvas_draw_str(canvas, 4, 19, "GAME OVER");
        canvas_draw_xbm(canvas, 74, 6, 51, 54, rip_glyph);

        if (state->suicide) {
            canvas_draw_str(canvas, 4, 38, "You have");
            canvas_draw_str(canvas, 4, 49, "blown yourself");
            canvas_draw_str(canvas, 4, 60, "up.");
        } else {
            canvas_draw_str(canvas, 4, 38, "You have been");
            canvas_draw_str(canvas, 4, 49, "blown up by");
            canvas_draw_str(canvas, 4, 60, "the other player");
        }
    } else {
        canvas_draw_str(canvas, 4, 19, "YOU WIN!");
        canvas_draw_xbm(canvas, 71, 5, 46, 48, happyfops_glyph);

        if (state->suicide) {
            canvas_draw_str(canvas, 4, 38, "The other");
            canvas_draw_str(canvas, 4, 49, "player blew");
            canvas_draw_str(canvas, 4, 60, "themselves up.");
        } else {
            canvas_draw_str(canvas, 4, 38, "You have");
            canvas_draw_str(canvas, 4, 49, "blown up");
            canvas_draw_str(canvas, 4, 60, "the other player!");
        }
    }
}