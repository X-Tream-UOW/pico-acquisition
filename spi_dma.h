#ifndef SPI_DMA_H
#define SPI_DMA_H

#include <stdint.h>

extern int spi_dma_chan;
extern uint16_t* current_spi_buffer;

void setup_spi_dma(void);
void trigger_spi_dma(uint16_t* buffer);

#endif
