#include "pico/stdlib.h"
#include "gpio_init.h"

void init_gpio(void) {
    gpio_init(READY_PIN);
    gpio_set_dir(READY_PIN, GPIO_OUT);
    gpio_put(READY_PIN, 0);

    gpio_init(ACK_PIN);
    gpio_set_dir(ACK_PIN, GPIO_IN);
}

void clear_signals(void) {
    gpio_set_dir(ACK_PIN, GPIO_OUT);  
    gpio_put(ACK_PIN, 0);
    gpio_put(READY_PIN, 0);
    gpio_set_dir(ACK_PIN, GPIO_IN);
}
