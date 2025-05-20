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

void init_db_lines(PIO pio) {
    for (int i = 0; i < DB_COUNT; i++) {
        pio_gpio_init(pio, DB_STARTS + i);
    }
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Reader connected\n");

    // Initialize CS_PIN
    gpio_init(CS_PIN);
    gpio_set_dir(CS_PIN, GPIO_OUT);
    gpio_put(CS_PIN, 1);  // idle state high

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &reader_program);
    uint sm = pio_claim_unused_sm(pio, true);

    // Init control and data lines
    pio_gpio_init(pio, CONVST_PIN);
    pio_gpio_init(pio, RD_PIN);
    pio_gpio_init(pio, BUSY_PIN);
    init_db_lines(pio);

    // Set directions: CONVST and RD as outputs, BUSY and DB as inputs
    pio_sm_set_consecutive_pindirs(pio, sm, CONVST_PIN, 2, true);           // CONVST, RD
    pio_sm_set_consecutive_pindirs(pio, sm, BUSY_PIN, DB_COUNT + 1, false); // BUSY + DB[15:0]

    // Configure state machine
    pio_sm_config c = reader_program_get_default_config(offset);
    sm_config_set_set_pins(&c, CONVST_PIN, 1);             // PIO .set controls CONVST only
    sm_config_set_sideset_pins(&c, RD_PIN);                // RD via .sideset
    sm_config_set_in_pins(&c, BUSY_PIN);                   // input starts at BUSY
    sm_config_set_in_shift(&c, false, true, 16);           // shift 16 bits from ISR, autopush

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    while (true) {
        gpio_put(CS_PIN, 0);  // start acquisition window
        sleep_us(5);         

        printf("Initiating sampling...\n");
        pio_sm_exec(pio, sm, pio_encode_jmp(offset));

        while (pio_sm_is_rx_fifo_empty(pio, sm))
            tight_loop_contents();

        uint32_t raw = pio_sm_get(pio, sm);

        uint16_t adc_val = (raw >> 1) & 0xFFFF;
        uint8_t msb = (adc_val >> 8) & 0xFF;
        uint8_t lsb = adc_val & 0xFF;

        printf("RAW:  0x%05X\n", raw);
        printf("ADC:  0x%04X (%5u)\n", adc_val, adc_val);
        printf("MSB:  0x%02X, LSB: 0x%02X\n", msb, lsb);

        sleep_ms(200);
    }
}
