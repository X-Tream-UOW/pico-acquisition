#include "hardware/dma.h"
#include "hardware/irq.h"
#include "adc_dma.h"
#include "spi_dma.h"

void dma_irq_handler() {
    uint32_t status = dma_hw->ints0;

    if (status & (1u << reader_dma_chan)) {
        dma_hw->ints0 = 1u << reader_dma_chan;
        reader_dma_handler();
    }

    if (status & (1u << spi_dma_chan)) {
        dma_hw->ints0 = 1u << spi_dma_chan;
        spi_dma_handler();
    }
}

void setup_dma_irq_handler() {
    irq_set_exclusive_handler(DMA_IRQ_0, dma_irq_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}
