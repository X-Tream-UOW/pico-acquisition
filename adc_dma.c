#include "hardware/dma.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "adc_dma.h"
#include "reader.pio.h"
#include "pio_reader_sm.h"
#include "gpio_init.h"
#include "spi_dma.h"
#include <stdio.h>

uint16_t buffer1[BUFFER_SIZE];
uint16_t buffer2[BUFFER_SIZE];

volatile bool buffer1_ready = false;
volatile bool buffer2_ready = false;
volatile bool reader_dma_paused = false;

int reader_dma_chan = -1;
extern PIO pio;
extern uint sm_reader;

void __isr reader_dma_handler() {
    dma_hw->ints0 = 1u << reader_dma_chan;

    static bool last_used_buffer1 = false;

    if (!last_used_buffer1) {
        buffer1_ready = true;
        gpio_put(READY_PIN, 1);
        printf("[ADC DMA] Buffer 1 filled and ready to send\n");

        if (!buffer2_ready) {
            dma_channel_set_write_addr(reader_dma_chan, buffer2, false);
            dma_channel_set_trans_count(reader_dma_chan, BUFFER_SIZE, true);
            last_used_buffer1 = true;
            return;
        }

    } else {
        buffer2_ready = true;
        gpio_put(READY_PIN, 1);
        printf("[ADC DMA] Buffer 2 filled and ready to send\n");

        if (!buffer1_ready) {
            dma_channel_set_write_addr(reader_dma_chan, buffer1, false);
            dma_channel_set_trans_count(reader_dma_chan, BUFFER_SIZE, true);
            last_used_buffer1 = false;
            return;
        }
    }

    // Both buffers are full → pause and try to resume
    reader_dma_paused = true;
    printf("[ADC DMA] Both buffers full — pausing acquisition\n");

    // Edge fix: just try resuming in case something cleared
    resume_reader_dma();
}

void resume_reader_dma(void) {
    if (!reader_dma_paused) return;

    if (!buffer1_ready) {
        reader_dma_paused = false;
        dma_channel_set_write_addr(reader_dma_chan, buffer1, false);
    } else if (!buffer2_ready) {
        reader_dma_paused = false;
        dma_channel_set_write_addr(reader_dma_chan, buffer2, false);
    } else {
        // Still no free buffer: keep paused
        return;
    }

    dma_channel_set_trans_count(reader_dma_chan, BUFFER_SIZE, true);
    dma_channel_start(reader_dma_chan);
}


void setup_reader_dma() {

    gpio_init(READY_PIN);
    gpio_set_dir(READY_PIN, GPIO_OUT);

    reader_dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(reader_dma_chan);

    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm_reader, false));

    dma_channel_configure(
        reader_dma_chan,
        &cfg,
        buffer1,
        (const volatile uint16_t *)&pio->rxf[sm_reader],
        BUFFER_SIZE,
        false
    );

    dma_channel_set_irq0_enabled(reader_dma_chan, true);
}
