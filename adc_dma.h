#pragma once
#include <stdint.h>
#include <stdbool.h>

extern uint16_t buffer1[];
extern uint16_t buffer2[];
extern volatile bool buffer1_ready;
extern volatile bool buffer2_ready;

void setup_dma();
