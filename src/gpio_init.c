#include "pico/stdlib.h"
#include "gpio_init.h"
#include <stdio.h>

void init_gpios(void) {
    gpio_init(READY_PIN);
    gpio_set_dir(READY_PIN, GPIO_OUT);
    gpio_put(READY_PIN, 0);
}

