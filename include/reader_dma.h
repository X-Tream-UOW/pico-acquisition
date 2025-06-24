#pragma once

#include <stdint.h>
#include <stdbool.h>

extern int reader_dma_chan;

void setup_reader_dma(void);
void stop_reader_dma(void);
void reader_dma_handler(void);
