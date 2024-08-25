#ifndef LEVELS_H
#define LEVELS_H

#include <stdint.h>

extern uint8_t level1[128];
extern uint8_t level2[128];
extern uint8_t level3[128];
extern uint8_t level4[128];
extern uint8_t level5[128];

uint8_t* levels[] = { level1, level2, level3, level4, level5 };

#endif