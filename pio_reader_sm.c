#include "hardware/pio.h"
#include "pio_reader_sm.h"
#include "reader.pio.h"
#include "gpio_init.h"

#define DB_STARTS    0
#define DB_COUNT     16
#define BUSY_PIN     16
#define CS_PIN       17
#define CONVST_PIN   18
#define RD_PIN       19

PIO pio = pio0;
uint sm_reader = 0;

void setup_reader_sm(void) {
    uint offset_reader = pio_add_program(pio, &reader_program);
    sm_reader = pio_claim_unused_sm(pio, true);

    for (int i = 0; i < DB_COUNT; i++)
        pio_gpio_init(pio, DB_STARTS + i);

    pio_gpio_init(pio, BUSY_PIN);
    pio_gpio_init(pio, CS_PIN);
    pio_gpio_init(pio, CONVST_PIN);
    pio_gpio_init(pio, RD_PIN);

    pio_sm_config c_reader = reader_program_get_default_config(offset_reader);
    sm_config_set_set_pins(&c_reader, CS_PIN, 3);  // CS, CONVST, RD
    sm_config_set_in_pins(&c_reader, DB_STARTS);
    sm_config_set_in_shift(&c_reader, false, true, 16);
    sm_config_set_fifo_join(&c_reader, PIO_FIFO_JOIN_RX);

    pio_sm_init(pio, sm_reader, offset_reader, &c_reader);

    pio_sm_set_consecutive_pindirs(pio, sm_reader, CS_PIN, 3, true);
    pio_sm_set_consecutive_pindirs(pio, sm_reader, DB_STARTS, DB_COUNT, false);
    pio_sm_set_consecutive_pindirs(pio, sm_reader, BUSY_PIN, 1, false);

    pio_sm_set_enabled(pio, sm_reader, true);
}
