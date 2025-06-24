#include <stdio.h>
#include "pico/stdlib.h"

#include "gpio_init.h"
#include "gpio_ack.h"
#include "pwm_trigger.h"
#include "pio_reader_sm.h"
#include "spi_slave_sm.h"

#define ACQ_FREQ 1000000.0f

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Connected\n");

    init_gpios();
    setup_gpio_irq(); 
    setup_pwm(ACQ_FREQ);

    setup_reader_sm();
    setup_spi_sm();

    while (true) {
        tight_loop_contents();
    }
}
