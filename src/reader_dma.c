#include "buffer_manager.h"
#include "hardware/dma.h"
#include "hardware/pio.h"
#include <stdio.h>

extern PIO pio;
extern uint sm_reader;
int reader_dma_chan = -1;

static uint16_t* current_buffer = NULL;

void reader_dma_handler(void) {  // This is triggered when a buffer is filled
    buffer_manager_mark_full(current_buffer);  // Mark the just-filled buffer full

    current_buffer = buffer_manager_get_free_buffer();  // Get next available buffer
    if (current_buffer) {
        dma_channel_set_write_addr(reader_dma_chan, current_buffer, false);
        dma_channel_set_trans_count(reader_dma_chan, BUFFER_SIZE, true);
    }
}

void setup_reader_dma(void) {
    reader_dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(reader_dma_chan);

    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm_reader, false));

    current_buffer = buffer_manager_get_free_buffer();
    if (!current_buffer) {
        return;
    }

    dma_channel_configure(
        reader_dma_chan,
        &cfg,
        current_buffer,                  // Destination buffer
        &pio->rxf[sm_reader],            // Source: RX FIFO
        BUFFER_SIZE,
        false
    );

    dma_channel_set_irq0_enabled(reader_dma_chan, true);
}
