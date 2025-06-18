#ifndef ADC_DMA_H
#define ADC_DMA_H

#include <stdint.h>
#include <stdbool.h>

#define BUFFER_SIZE 256

extern uint16_t buffer1[BUFFER_SIZE];
extern uint16_t buffer2[BUFFER_SIZE];

extern volatile bool buffer1_ready;
extern volatile bool buffer2_ready;
extern volatile bool dma_paused;

void setup_dma(void);
void resume_dma(void);

#endif