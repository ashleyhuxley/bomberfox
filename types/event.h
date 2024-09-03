#ifndef EVENT_H
#define EVENT_H

#include <notification/notification_messages.h>

// Event types for message queue
typedef enum {
    BomberEventType_Input,
    BomberEventType_Tick,
    BomberEventType_SubGhz,
    BomberEventType_HaveLevelData
} BomberEventType;

typedef struct {
    BomberEventType type;
    InputEvent input;
    size_t subGhzIncomingSize;
} BomberEvent;

#endif