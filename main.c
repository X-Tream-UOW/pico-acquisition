#include <stdio.h>
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "reader.pio.h"
#include "hardware/dma.h"

#define BUFFER_SIZE 65536

uint16_t buffer1[BUFFER_SIZE];
uint16_t buffer2[BUFFER_SIZE];
volatile bool buffer1_ready = false;
volatile bool buffer2_ready = false;

#define DB_STARTS    0
#define DB_COUNT     16
#define BUSY_PIN     16
#define CS_PIN       17
#define CONVST_PIN   18
#define RD_PIN       19
#define MEASURE_INTERVAL_MS 1000

// # define PI_MISO 20
// # define PI_READY 21
// # define PI_CLK 22
// # define PI_ACK 26
// # define PI_CS 27
// # define PI_ACQ 28

int dma_chan;

void __isr dma_handler() {
    dma_hw->ints0 = 1u << dma_chan; // Clear interrupt

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

void setup_dma(PIO pio, uint sm) {
    dma_chan = dma_claim_unused_channel(true);
    dma_channel_config cfg = dma_channel_get_default_config(dma_chan);

    channel_config_set_read_increment(&cfg, false);
    channel_config_set_write_increment(&cfg, true);
    channel_config_set_dreq(&cfg, pio_get_dreq(pio, sm, false));  // PIO RX FIFO

    channel_config_set_transfer_data_size(&cfg, DMA_SIZE_16);  // 16-bit transfers

    dma_channel_configure(
        dma_chan,
        &cfg,
        buffer1,                                        // First target buffer
        (const volatile uint16_t *)&pio->rxf[sm],       // Source: PIO RX FIFO
        BUFFER_SIZE,                                    // Transfer count
        false                                           // Don't start yet
    );

    // Set up interrupt handler
    dma_channel_set_irq0_enabled(dma_chan, true);
    irq_set_exclusive_handler(DMA_IRQ_0, dma_handler);
    irq_set_enabled(DMA_IRQ_0, true);
}


void init_db_lines(PIO pio) {
    for (int i = 0; i < DB_COUNT; i++) {
        pio_gpio_init(pio, DB_STARTS + i);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Reader connected\n");

    // Init PIO and pins
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &reader_program);
    uint sm = pio_claim_unused_sm(pio, true);

    // Init all data pins
    init_db_lines(pio);

    // Init control and BUSY pins
    pio_gpio_init(pio, BUSY_PIN);
    pio_gpio_init(pio, CS_PIN);
    pio_gpio_init(pio, CONVST_PIN);
    pio_gpio_init(pio, RD_PIN);

    // Set pin directions
    pio_sm_set_consecutive_pindirs(pio, sm, CS_PIN, 3, true);  // CS, CONVST, RD as outputs
    pio_sm_set_consecutive_pindirs(pio, sm, DB_STARTS, DB_COUNT + 1, false);  // DB[0..15] & BUSY as inputs

    // Configure the state machine
    pio_sm_config c = reader_program_get_default_config(offset);
    sm_config_set_set_pins(&c, CS_PIN, 3);    // 3 control lines starting from CS
    sm_config_set_in_pins(&c, DB_STARTS);      // For in pins, 17
    sm_config_set_in_shift(&c, false, true, 16);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
    
    setup_dma(pio, sm);
    dma_channel_start(dma_chan);

    // Measure acquisition frequency
    const uint64_t start_time = time_us_64();
    uint64_t last_report_time = start_time;
    uint32_t sample_count = 0;

    uint64_t dma_start_time = time_us_64();

    while (true) {
        
        if (buffer1_ready) {
            buffer1_ready = false;
            uint64_t now = time_us_64();
            printf("buffer1 filled in %llu us\n", now - dma_start_time);
            dma_start_time = now;
            // Process buffer1...
        }
        
        if (buffer2_ready) {
            buffer2_ready = false;
            uint64_t now = time_us_64();
            printf("buffer2 filled in %llu us\n", now - dma_start_time);
            dma_start_time = now;
            // Process buffer2...
        }
    }

}