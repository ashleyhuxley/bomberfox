#include "modules/playerselect.h"

// Renders the player select menu to the viewport - called while in the player select menu
void bomber_playerselect_render(Canvas* canvas, BomberAppState* state) {
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

// Handles input while on player select screen - just switch between Fox/Wolf
// state: Pointer to the application state
// input: Represents the input event
// returns: true if the viewport should be updated, else false
bool bomber_playerselect_input(BomberAppState* state, InputEvent input) {
    if(input.type == InputTypeShort) {
        switch(input.key) {
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyLeft:
        case InputKeyRight:
            state->isPlayerTwo = !state->isPlayerTwo;
            return true;
        case InputKeyOk:
            if (!state->isPlayerTwo) {
                bomber_app_select_level(state);
            } else {
                bomber_app_wait(state);
            }
            return true;
        default:
            return false;
        }
    }
    return false;
}