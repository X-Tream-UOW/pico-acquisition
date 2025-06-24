#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "spi_slave.pio.h"
#include "pio_reader_sm.h"
#include <stdio.h>

#define MISO_PIN 20

#define CS_PIN   27
#define SCK_PIN  22

extern PIO pio;
uint sm_spi = 0;
uint offset_spi = -1;

void setup_spi_sm(void) {
    offset_spi = pio_add_program(pio, &spi_slave_program);
    sm_spi = pio_claim_unused_sm(pio, true);

    pio_gpio_init(pio, CS_PIN);
    pio_gpio_init(pio, SCK_PIN);
    pio_gpio_init(pio, MISO_PIN);

    uint32_t pin_mask_output = 1u << MISO_PIN;
    uint32_t pin_mask_input = (1u << CS_PIN) | (1u << SCK_PIN);
    uint32_t full_mask = pin_mask_output | pin_mask_input;

    pio_sm_set_pindirs_with_mask(pio, sm_spi, pin_mask_output, full_mask); 

    pio_sm_config c = spi_slave_program_get_default_config(offset_spi);

    sm_config_set_out_pins(&c, MISO_PIN, 1);
    sm_config_set_out_shift(&c, false, true, 16);  // MSB-first
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);

    pio_sm_init(pio, sm_spi, offset_spi, &c);
}

void clean_and_start_spi_sm(void) {
    pio_sm_clear_fifos(pio, sm_spi);
    pio_sm_set_enabled(pio, sm_spi, true);
}

void stop_and_clear_spi_sm(void) {
    pio_sm_set_enabled(pio, sm_spi, false);
    pio_sm_clear_fifos(pio, sm_spi);
}

