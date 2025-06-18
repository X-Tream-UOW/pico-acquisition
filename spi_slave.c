#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "spi_slave.pio.h"
#include <stdio.h>

#define MISO_PIN 20

#define CS_PIN   27
#define SCK_PIN  22

int setup_spi() {

    PIO pio = pio0;
    uint offset = pio_add_program(pio, &spi_slave_program);
    uint sm = pio_claim_unused_sm(pio, true);

    pio_gpio_init(pio, CS_PIN);
    pio_gpio_init(pio, SCK_PIN);
    pio_gpio_init(pio, MISO_PIN);

    uint32_t pin_mask_output = 1u << MISO_PIN;
    uint32_t pin_mask_input = (1u << CS_PIN) | (1u << SCK_PIN);
    uint32_t full_mask = pin_mask_output | pin_mask_input;

    pio_sm_set_pindirs_with_mask(pio, sm, pin_mask_output, full_mask); 

    pio_sm_config c = spi_slave_program_get_default_config(offset);

    sm_config_set_in_pins(&c, SCK_PIN);
    sm_config_set_out_pins(&c, MISO_PIN, 1);
    sm_config_set_out_shift(&c, false, true, 8);  // MSB-first

    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    uint8_t counter = 0;
}

