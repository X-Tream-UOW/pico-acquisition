#pragma once

#include <stdint.h>
#include <stdbool.h>

extern int reader_dma_chan;

void setup_reader_dma(void);
void __isr reader_dma_handler(void);
