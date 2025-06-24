#include <stdio.h>
#include "pico/stdlib.h"

#include "gpio_init.h"
#include "gpio_ack.h"
#include "pwm_trigger.h"
#include "pio_reader_sm.h"
#include "spi_slave_sm.h"
#include "acquisition_control.h"
#include "buffer_manager.h"

#define ACQ_FREQ 1000000.0f

extern uint16_t buffer1[BUFFER_SIZE];
extern uint16_t buffer2[BUFFER_SIZE];

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("Connected\n");

    init_gpios();
    setup_ack_gpio();
    setup_pwm(ACQ_FREQ);

    setup_reader_sm();
    setup_spi_sm();

    sleep_ms(100);

    start_acquisition();
    sleep_ms(1000);
    stop_acquisition();

    sleep_ms(1000);
    
    start_acquisition();
    sleep_ms(1000);
    stop_acquisition();

    while (true) {
        tight_loop_contents();
    }
}
