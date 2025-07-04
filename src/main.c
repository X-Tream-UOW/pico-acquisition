#include <stdio.h>
#include "pico/stdlib.h"

#include "gpio_init.h"
#include "gpio_ack.h"
#include "pwm_trigger.h"
#include "pio_reader_sm.h"
#include "spi_slave_sm.h"

#include "acquisition_control.h"
#include "hardware/clocks.h"

#define ACQ_FREQ 1000000.0f

int main() {
    set_sys_clock_khz(300000, false);

    init_gpios();
    setup_gpio_irq(); 
    setup_pwm(ACQ_FREQ);

    setup_reader_sm();
    setup_spi_sm();

    while (true) {
        tight_loop_contents();
    }
}
