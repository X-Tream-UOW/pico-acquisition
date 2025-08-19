/* This file implements the interrupt handler for the DMA. This allows to time, start and stop the
DMA transfer with precision and ensure data integrity. */

#include "hardware/dma.h"
#include "hardware/irq.h"
#include "reader_dma.h"
#include "spi_dma.h"

void __isr dma_irq_handler() {
    uint32_t status = dma_hw->ints0;

    // Detect which channel triggered the interrupt

    if (status & (1u << reader_dma_chan)) {
        dma_hw->ints0 = 1u << reader_dma_chan;
        reader_dma_handler();
    }

    if (status & (1u << spi_dma_chan)) {
        dma_hw->ints0 = 1u << spi_dma_chan;
    }
}

void setup_dma_irq_handler() {
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}
