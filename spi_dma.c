#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "adc_dma.h"

int dma_chan;

void start_spi_dma_transfer(PIO pio, uint sm, uint32_t *buffer) {
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_chan);

    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);  // walk through the buffer
    channel_config_set_write_increment(&c, false); // write to same TX FIFO address
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, true)); // DREQ on TX FIFO

    dma_channel_configure(
        dma_chan,
        &c,
        &pio->txf[sm],   // write addr (PIO TX FIFO)
        buffer,          // read addr (your buffer)
        BUFFER_SIZE,     // number of 32-bit words
        true             // start immediately
    );
}
