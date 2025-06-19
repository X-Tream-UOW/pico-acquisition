#ifndef ADC_DMA_H
#define ADC_DMA_H

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 65536

extern uint16_t buffer1[BUFFER_SIZE];
extern uint16_t buffer2[BUFFER_SIZE];

extern volatile bool buffer1_ready;
extern volatile bool buffer2_ready;
extern volatile bool reader_dma_paused;

extern int reader_dma_chan;

void setup_reader_dma(void);
void resume_reader_dma(void);
void __isr reader_dma_handler();

#endif