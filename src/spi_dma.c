/* This file implements the DMA transfer from the RAM buffers to the SPI PIO state machine. */

#include "buffer_manager.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "gpio_init.h"
#include <stdio.h>

extern PIO pio;
extern uint sm_spi;

int spi_dma_chan = -1;
uint16_t* current_spi_buffer = NULL;

void setup_spi_dma(void) {
    if (spi_dma_chan < 0) {
        spi_dma_chan = dma_claim_unused_channel(true);
    }

    dma_channel_config cfg = dma_channel_get_default_config(spi_dma_chan);

    channel_config_set_read_increment(&cfg, true);  // We increment the read adress to parse the buffer
    channel_config_set_write_increment(&cfg, false); // We always write to the SM fifo
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);  // 16-bits samples
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm_spi, true));

    dma_channel_configure(
        spi_dma_chan,
        &cfg,
        &pio->txf[sm_spi],         // write address (fixed)
        NULL,                      // read address will be set in trigger
        BUFFER_SIZE,
        false                      // do not start yet
    );
}

void stop_spi_dma(void) {
    dma_channel_set_irq0_enabled(spi_dma_chan, false);
    dma_hw->ints0 = 1u << spi_dma_chan;  // Clear any pending IRQ
    dma_channel_abort(spi_dma_chan);
}

void trigger_spi_dma(uint16_t* buffer) {  // triggers a new transfer from the specified buffer
    current_spi_buffer = buffer;
    dma_channel_abort(spi_dma_chan);

    dma_channel_set_read_addr(spi_dma_chan, buffer, false);
    dma_channel_set_trans_count(spi_dma_chan, BUFFER_SIZE, true);
}
