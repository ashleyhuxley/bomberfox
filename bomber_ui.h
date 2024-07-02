#ifndef BOMBER_UI_H
#define BOMBER_UI_H

#include "types.h"

void bomber_ui_render_callback(Canvas* canvas, void* context);
void bomber_ui_input_callback(InputEvent* input_event, void* context_q);

#endif