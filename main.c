#include <stdio.h>
#include "hardware/pio.h"
#include "pico/stdlib.h"
#include "reader.pio.h"

#define CONVST_PIN 0
#define RD_PIN     1
#define BUSY_PIN   2
#define DB_STARTS  4
#define DB_COUNT   16
#define CS_PIN     22
#define MEASURE_INTERVAL_MS 1000

void init_db_lines(PIO pio) {
    for (int i = 0; i < DB_COUNT; i++) {
        pio_gpio_init(pio, DB_STARTS + i);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Reader connected\n");

    // Init CS
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 0);

    // Init PIO
    PIO pio = pio0;
    uint offset = pio_add_program(pio, &reader_program);
    uint sm = pio_claim_unused_sm(pio, true);

    pio_gpio_init(pio, CONVST_PIN);
    pio_gpio_init(pio, RD_PIN);
    pio_gpio_init(pio, BUSY_PIN);
    init_db_lines(pio);

    pio_sm_set_consecutive_pindirs(pio, sm, CONVST_PIN, 2, true);
    pio_sm_set_consecutive_pindirs(pio, sm, BUSY_PIN, DB_COUNT + 1, false); 

    pio_sm_config c = reader_program_get_default_config(offset);
    sm_config_set_set_pins(&c, CONVST_PIN, 1);
    sm_config_set_sideset_pins(&c, RD_PIN);
    sm_config_set_in_pins(&c, BUSY_PIN);
    sm_config_set_in_shift(&c, false, true, 16);

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    // Sampling loop
    const uint64_t start_time = time_us_64();
    uint64_t last_report_time = start_time;
    uint32_t sample_count = 0;

    while (true) {
        pio_sm_exec(pio, sm, pio_encode_jmp(offset));
        while (pio_sm_is_rx_fifo_empty(pio, sm))
            tight_loop_contents();

        pio_sm_get(pio, sm);
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
