#include <stdio.h>
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "reader.pio.h"

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
    sm_config_set_in_pins(&c, BUSY_PIN);      // For in pins, 17
    sm_config_set_in_shift(&c, false, true, 16);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    // Measure acquisition frequency
    const uint64_t start_time = time_us_64();
    uint64_t last_report_time = start_time;
    uint32_t sample_count = 0;

    while (true) {
        while (pio_sm_is_rx_fifo_empty(pio, sm))
            tight_loop_contents();

        uint32_t sample = pio_sm_get(pio, sm);
        sample_count++;

        uint64_t now = time_us_64();
        if ((now - last_report_time) >= (MEASURE_INTERVAL_MS * 1000)) {
            float seconds = (now - last_report_time) / 1e6f;
            float frequency = sample_count / seconds;
            printf("Acquisition rate: %.2f samples/sec\n", frequency);

            last_report_time = now;
            sample_count = 0;
        }
    }
}