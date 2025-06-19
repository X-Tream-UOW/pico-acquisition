#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include "adc_dma.h"
#include "spi_slave_sm.h"

bool use_buffer1 = true;

int spi_dma_chan = -1;

void spi_dma_handler() {
    dma_hw->ints0 = 1u << spi_dma_chan;  // Clear IRQ

    use_buffer1 = !use_buffer1;
    uint16_t* next_buf = use_buffer1 ? buffer1 : buffer2;

    dma_channel_set_read_addr(spi_dma_chan, next_buf, true);
}

void setup_spi_dma() {
    spi_dma_chan = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(spi_dma_chan);

    channel_config_set_transfer_data_size(&c, DMA_SIZE_16);
    channel_config_set_read_increment(&c, true);  // walk through the buffer
    channel_config_set_write_increment(&c, false); // write to same TX FIFO address
    channel_config_set_dreq(&c, pio_get_dreq(spi_pio, sm_spi, false)); // DREQ on TX FIFO

    dma_channel_configure(
        spi_dma_chan,
        &c,
        &spi_pio->txf[sm_spi],   // write addr (PIO TX FIFO)
        buffer1,         // read addr (your buffer)
        BUFFER_SIZE,     // number of 16-bit words
        true             // start immediately
    );

    dma_channel_set_irq0_enabled(spi_dma_chan, true);
}


