#include "pico/stdlib.h"
#include "hardware/dma.h"
#include "hardware/irq.h"
#include "gpio_init.h"

#define BUFFER_SIZE 65536

uint16_t buffer1[BUFFER_SIZE];
uint16_t buffer2[BUFFER_SIZE];
volatile bool buffer1_ready = false;
volatile bool buffer2_ready = false;
int dma_chan;

void __isr dma_handler() {
    dma_hw->ints0 = 1u << dma_chan;
    static bool ping = false;
    if (ping) {
        buffer2_ready = true;
        dma_channel_set_write_addr(dma_chan, buffer1, true);
    } else {
        buffer1_ready = true;
        dma_channel_set_write_addr(dma_chan, buffer2, true);
    }
    ping = !ping;
}

void setup_dma() {
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm_reader, false));

    dma_channel_configure(
        dma_chan,
        &cfg,
        buffer1,
        (const volatile uint16_t *)&pio->rxf[sm_reader],
        BUFFER_SIZE,
        false
    );

    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);

    dma_channel_start(dma_chan);
}

