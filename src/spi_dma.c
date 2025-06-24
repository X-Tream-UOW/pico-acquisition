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
    spi_dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(spi_dma_chan);

    channel_config_set_read_increment(&cfg, true);
    channel_config_set_write_increment(&cfg, false);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm_spi, true));

    dma_channel_set_config(spi_dma_chan, &cfg, false);
}

void trigger_spi_dma(uint16_t* buffer) {
    current_spi_buffer = buffer;
    dma_channel_set_write_addr(spi_dma_chan, &pio->txf[sm_spi], false);
    dma_channel_set_read_addr(spi_dma_chan, buffer, false);
    dma_channel_set_trans_count(spi_dma_chan, BUFFER_SIZE, true);
}
