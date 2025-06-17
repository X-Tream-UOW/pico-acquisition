#pragma once
#include "hardware/pio.h"

extern PIO pio;
extern uint sm_reader;

void init_pio_and_gpio();
